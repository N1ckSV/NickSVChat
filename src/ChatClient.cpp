
#include "NickSV/Chat/ChatClient.h"

#include <steam/steamnetworkingsockets.h>

#include "NickSV/Chat/ChatSocket.h"

#include "NickSV/Chat/Requests/Request.h"
#include "NickSV/Chat/ClientInfo.h"

#include "NickSV/Chat/Serializers/ClientInfoRequestSerializer.h"
#include "NickSV/Chat/Serializers/MessageRequestSerializer.h"

#include <iostream>


namespace NickSV::Chat {

EResult ChatClient::Run(const ChatIPAddr &serverAddr)
{
	ChatErrorMsg errMsg;
	EResult result = OnPreRun(serverAddr, errMsg);
	if(result != EResult::Success)
		return OnRun(serverAddr, result, errMsg);

	result = ChatSocket::Run(serverAddr);
	if(result != EResult::Success)
		return OnRun(serverAddr, result, errMsg);
	
	if (!GameNetworkingSockets_Init(nullptr, errMsg))
        return OnRun(serverAddr, EResult::Error, errMsg);

    m_pInterface = SteamNetworkingSockets();
	SteamNetworkingConfigValue_t opt;
	opt.SetPtr(k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged, (void*)SteamNetConnectionStatusChangedCallback);
    m_hConnection = m_pInterface->ConnectByIPAddress(serverAddr, 1,&opt);    
    if (m_hConnection == k_HSteamNetConnection_Invalid)
	{
		GameNetworkingSockets_Kill();
		sprintf(errMsg, "ISteamNetworkingSockets interface returned invalid connection");
        return OnRun(serverAddr, EResult::Error, errMsg);
	}

    m_bGoingExit = false; //SHOULD BE ALWAYS FIRST BEFORE m_pConnectionThread = new ...
    m_pConnectionThread = new std::thread(&ChatClient::ConnectionThreadFunction, this);
    m_pRequestThread = new std::thread(&ChatClient::RequestThreadFunction, this);
    //TODO: exit/kill/nuke/terminate

    ///////////////////////////////////
    return OnRun(serverAddr, EResult::Success, errMsg);
}

ChatClient::ChatClient() : ChatSocket()
{
	m_pClientInfo = new ClientInfo();
}

ChatClient::~ChatClient()
{
    //TODO
    //FIXME FIXME FIXME FIXME FIXME FIXME 
	delete m_pClientInfo;
}

EResult ChatClient::PollIncomingRequests()
{
	while (!m_bGoingExit)
	{
		
		if(m_handleRequestsQueue.size() >= Constant::MaxSendRequestsQueueSize)
			return EResult::Overflow;
			
		ISteamNetworkingMessage *pIncomingMsg = nullptr;
		int numMsgs = m_pInterface->ReceiveMessagesOnConnection(m_hConnection, &pIncomingMsg, 1);
		if (numMsgs == 0)
			return EResult::Success;
		if (numMsgs < 0)
			return EResult::Error;
		
		if((size_t)pIncomingMsg->m_cbSize < sizeof(ERequestType))
			return EResult::Error; //FIXME mb return EResult::CorruptedData

		std::string strReq;
		strReq.assign(static_cast<const char *>(pIncomingMsg->m_pData), pIncomingMsg->m_cbSize);

		std::pair<std::string, RequestInfo> para(std::move(strReq), {pIncomingMsg->m_conn, 0});
		m_handleRequestMutex.lock();
		m_handleRequestsQueue.push(std::move(para));
		m_handleRequestMutex.unlock();
		pIncomingMsg->Release();
	}
	return EResult::Success;
}

EResult ChatClient::PollQueuedRequests()
{
	auto result = EResult::Success;
	while (!m_bGoingExit && !m_sendRequestsQueue.empty())
	{
		std::lock_guard<std::mutex> lock(m_sendRequestMutex);
		if(SendStringToServer(std::move(m_sendRequestsQueue.front().first)) == EResult::Error && result == EResult::Success)
				result = EResult::Error;
		
		m_sendRequestsQueue.pop();
	}
	return result;
}

void ChatClient::PollConnectionChanges()
{
	s_pCallbackInstance = this;
	m_pInterface->RunCallbacks();
}

EResult ChatClient::SendStringToServer(std::string str)
{
	auto result = m_pInterface->SendMessageToConnection(m_hConnection, str.data(), (uint32)str.size(), 
		k_nSteamNetworkingSend_Reliable, nullptr);
	return (result == k_EResultOK) ? EResult::Success : EResult::Error;
}

void ChatClient::ConnectionThreadFunction()
{
    ChatSocket::ConnectionThreadFunction();
	m_pInterface->CloseConnection(m_hConnection, 0, "Closing con", true);
}

void ChatClient::RequestThreadFunction()
{
    while(!m_bGoingExit)
    {
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		//TODO FIXME
    }
}

ChatClient *ChatClient::s_pCallbackInstance = nullptr;

void ChatClient::SteamNetConnectionStatusChangedCallback(SteamNetConnectionStatusChangedCallback_t *pInfo)
{
	s_pCallbackInstance->OnSteamNetConnectionStatusChanged(pInfo);
}

void ChatClient::OnSteamNetConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t *pInfo)
{	
	CHAT_ASSERT(pInfo->m_hConn == m_hConnection || m_hConnection == k_HSteamNetConnection_Invalid, "");

	// What's the state of the connection?
	switch (pInfo->m_info.m_eState)
	{
		case k_ESteamNetworkingConnectionState_None:
			// NOTE: We will get callbacks here when we destroy connections.  You can ignore these.
			break;

		case k_ESteamNetworkingConnectionState_ClosedByPeer:
		case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
		{
			m_bGoingExit = true;

			// Print an appropriate message
			if (pInfo->m_eOldState == k_ESteamNetworkingConnectionState_Connecting)
			{
				// Note: we could distinguish between a timeout, a rejected connection,
				// or some other transport problem.
				//Printf("We sought the remote host, yet our efforts were met with defeat.  (%s)", pInfo->m_info.m_szEndDebug);
			}
			else if (pInfo->m_info.m_eState == k_ESteamNetworkingConnectionState_ProblemDetectedLocally)
			{
				//Printf("Alas, troubles beset us; we have lost contact with the host.  (%s)", pInfo->m_info.m_szEndDebug);
			}
			else
			{
				// NOTE: We could check the reason code for a normal disconnection
				//Printf("The host hath bidden us farewell.  (%s)", pInfo->m_info.m_szEndDebug);
			}

			// Clean up the connection.  This is important!
			// The connection is "closed" in the network sense, but
			// it has not been destroyed.  We must close it on our end, too
			// to finish up.  The reason information do not matter in this case,
			// and we cannot linger because it's already closed on the other end,
			// so we just pass 0's.
			m_pInterface->CloseConnection(pInfo->m_hConn, 0, nullptr, false);
			m_hConnection = k_HSteamNetConnection_Invalid;
			break;
		}

		case k_ESteamNetworkingConnectionState_Connecting:
			// We will get this callback when we start connecting.
			// We can ignore this.
			break;

		case k_ESteamNetworkingConnectionState_Connected:
			//Printf("Connected to server OK");
			break;

		default:
			// Silences -Wswitch
			break;
	}
}
	




} /*END OF NAMESPACES*/