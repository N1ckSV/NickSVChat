#include "NickSV/Chat/ChatServer.h"

#include <steam/steamnetworkingsockets.h>

#include "NickSV/Chat/ChatSocket.h"

#include "NickSV/Chat/Requests/Request.h"
#include "NickSV/Chat/ClientInfo.h"

#include "NickSV/Chat/Serializers/ClientInfoRequestSerializer.h"
#include "NickSV/Chat/Serializers/MessageRequestSerializer.h"

#include "NickSV/Chat/Parsers/RequestParser.h"

#include <iostream>


namespace NickSV::Chat {

EResult ChatServer::Run(const ChatIPAddr &serverAddr)
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
	m_hListenSock = m_pInterface->CreateListenSocketIP(serverAddr, 1, &opt);
	if(m_hListenSock == k_HSteamListenSocket_Invalid)
	{
		GameNetworkingSockets_Kill();
		sprintf(errMsg, "ISteamNetworkingSockets interface created invalid listen socket");
        return OnRun(serverAddr, EResult::Error, errMsg);
	}

	m_hPollGroup = m_pInterface->CreatePollGroup();
	if(m_hPollGroup == k_HSteamNetPollGroup_Invalid)
	{
		GameNetworkingSockets_Kill();
		sprintf(errMsg, "ISteamNetworkingSockets interface created invalid poll group");
        return OnRun(serverAddr, EResult::Error, errMsg);
	}

    m_bGoingExit = false; //SHOULD BE ALWAYS FIRST BEFORE m_pConnectionThread = new ...
    m_pConnectionThread = new std::thread(&ChatServer::ConnectionThreadFunction, this);
    m_pRequestThread = new std::thread(&ChatServer::RequestThreadFunction, this);
    //TODO: exit/kill/nuke/terminate

    ///////////////////////////////////
    return OnRun(serverAddr, EResult::Success, errMsg);
}

ChatServer *ChatServer::s_pCallbackInstance = nullptr;
void ChatServer::SteamNetConnectionStatusChangedCallback(SteamNetConnectionStatusChangedCallback_t *pInfo)
{
	s_pCallbackInstance->OnSteamNetConnectionStatusChanged(pInfo);
}

ChatServer::ChatServer() : ChatSocket() {};

ChatServer::~ChatServer() {}

void ChatServer::ConnectionThreadFunction()
{
    ChatSocket::ConnectionThreadFunction();
	for (auto it: m_mapClients)
	{
		// Send them one more goodbye message.  Note that we also have the
		// connection close reason as a place to send final data.  However,
		// that's usually best left for more diagnostic/debug text not actual
		// protocol strings.
		SendStringToClient(it.first, "Server is shutting down.  Goodbye.");

		// Close the connection.  We use "linger mode" to ask SteamNetworkingSockets
		// to flush this out and close gracefully.
		m_pInterface->CloseConnection(it.first, 0, "Server Shutdown", true);
	}
	m_mapClients.clear();

	m_pInterface->CloseListenSocket(m_hListenSock);
	m_hListenSock = k_HSteamListenSocket_Invalid;

	m_pInterface->DestroyPollGroup(m_hPollGroup);
	m_hPollGroup = k_HSteamNetPollGroup_Invalid;
}


void ChatServer::RequestThreadFunction()
{
	RequestInfo rInfo;
	std::string strReq; 
	Parser<Request> parser;
	EResult result;
    while(!m_bGoingExit)
    {
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		if(m_handleRequestsQueue.empty())
			continue;
		
		std::lock_guard<std::mutex> lock(m_handleRequestMutex);
		while(!m_handleRequestsQueue.empty())
		{
			strReq = std::move(m_handleRequestsQueue.front().first);
			rInfo = m_handleRequestsQueue.front().second;
			parser.FromString(strReq);
			result = HandleRequest(parser.GetObject().get(), rInfo);
			(void)result; //FIXME
			m_handleRequestsQueue.pop();
		}
    }
}

EResult ChatServer::PollIncomingRequests()
{
	while (!m_bGoingExit)
	{
		if(m_handleRequestsQueue.size() >= Constant::MaxSendRequestsQueueSize)
			return EResult::Overflow;

		ISteamNetworkingMessage *pIncomingMsg = nullptr;
		int numMsgs = m_pInterface->ReceiveMessagesOnPollGroup(m_hPollGroup, &pIncomingMsg, 1);
		if (numMsgs == 0)
			return EResult::Success;
		if (numMsgs < 0)
			return EResult::Error;

		CHAT_ASSERT(numMsgs == 1, "Too many messages");
		CHAT_ASSERT(pIncomingMsg, "Pointer to message is nullptr");
		auto itClient = m_mapClients.find(pIncomingMsg->m_conn);
		CHAT_ASSERT(itClient != m_mapClients.end(), "Received message, but ChatServer has no info about connection");

		if((size_t)pIncomingMsg->m_cbSize < sizeof(ERequestType))
			return EResult::Error;

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

EResult ChatServer::PollQueuedRequests()
{
	auto result = EResult::Success;
	while (!m_bGoingExit && !m_sendRequestsQueue.empty())
	{
		m_sendRequestMutex.lock();
		std::string strRequest(std::move(m_sendRequestsQueue.front().first));
		RequestInfo rInfo = m_sendRequestsQueue.front().second;
		m_sendRequestsQueue.pop();
		m_sendRequestMutex.unlock();
		if((rInfo.sendFlags & SF_SEND_TO_ONE) > 0)
		{
			//SEND REQUEST TO RequestInfo::connection
			auto innerResult = SendStringToClient(rInfo.connection, std::move(strRequest));
			if(innerResult == EResult::Error && result == EResult::Success)
				result = EResult::Error;
			continue;
		}
		//SEND REQUEST TO EVERYONE EXCEPT RequestInfo::connection
		auto errors = SendStringToAllClients(std::move(strRequest), rInfo.connection);
		if(errors > 0 && result == EResult::Success)
			result = EResult::Error;
	}
	return result;
}

void ChatServer::PollConnectionChanges()
{
	s_pCallbackInstance = this;
	m_pInterface->RunCallbacks();
}

EResult ChatServer::SendStringToClient(HSteamNetConnection conn, std::string str)
{
	auto result = m_pInterface->SendMessageToConnection(conn, str.data(), (uint32)str.size(), 
		k_nSteamNetworkingSend_Reliable, nullptr);
	return (result == k_EResultOK) ? EResult::Success : EResult::Error;
}

size_t ChatServer::SendStringToAllClients(std::string str, HSteamNetConnection except)
{
	size_t errors = 0;
	for (auto &client: m_mapClients)
	{
		if (client.first != except)
			if(SendStringToClient(client.first, std::move(str)) == EResult::Error)
				errors++;
	}
	return errors;
}

UserID_t ChatServer::GenerateUniqueUserID()
{
	//FIXME FIXME FIXME FIXME 
	return 123;
}

void ChatServer::OnSteamNetConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t *pInfo)
{
	//char temp[1024];
	// What's the state of the connection?
	switch (pInfo->m_info.m_eState)
	{
		case k_ESteamNetworkingConnectionState_None:
			// NOTE: We will get callbacks here when we destroy connections.  You can ignore these.
			break;
		case k_ESteamNetworkingConnectionState_ClosedByPeer:
		case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
		{
			// Ignore if they were not previously connected.  (If they disconnected
			// before we accepted the connection.)
			if (pInfo->m_eOldState == k_ESteamNetworkingConnectionState_Connected)
			{
				// Locate the client.  Note that it should have been found, because this
				// is the only codepath where we remove clients (except on shutdown),
				// and connection change callbacks are dispatched in queue order.
				auto itClient = m_mapClients.find(pInfo->m_hConn);
				CHAT_ASSERT(itClient != m_mapClients.end(), "Empty m_mapClients");
				// Select appropriate log messages
				const char * pszDebugLogAction;
				if (pInfo->m_info.m_eState == k_ESteamNetworkingConnectionState_ProblemDetectedLocally)
				{
					pszDebugLogAction = "problem detected locally";
					//sprintf(temp, "Alas, %s hath fallen into shadow.  (%s)", itClient->second.m_sNick.c_str(), pInfo->m_info.m_szEndDebug);
				}
				else
				{
					// Note that here we could check the reason code to see if
					// it was a "usual" connection or an "unusual" one.
					pszDebugLogAction = "closed by peer";
					//sprintf(temp, " hath departed", itClient->second.m_sNick.c_str());
				}
				(void) pszDebugLogAction;
				// Spew something to our own log.  Note that because we put their nick
				// as the connection description, it will show up, along with their
				// transport-specific data (e.g. their IP address)
				//Printf("Connection %s %s, reason %d: %s\n",
				//	pInfo->m_info.m_szConnectionDescription,
				//	pszDebugLogAction,
				//	pInfo->m_info.m_eEndReason,
				//	pInfo->m_info.m_szEndDebug
				//);
				m_mapClients.erase(itClient);
				// Send a message so everybody else knows what happened
				//SendStringToAllClients(temp);
			}
			else
			{
				CHAT_ASSERT(pInfo->m_eOldState == k_ESteamNetworkingConnectionState_Connecting , "");
			}
			// Clean up the connection.  This is important!
			// The connection is "closed" in the network sense, but
			// it has not been destroyed.  We must close it on our end, too
			// to finish up.  The reason information do not matter in this case,
			// and we cannot linger because it's already closed on the other end,
			// so we just pass 0's.
			m_pInterface->CloseConnection(pInfo->m_hConn, 0, nullptr, false);
			break;
		}
		case k_ESteamNetworkingConnectionState_Connecting:
		{
			// This must be a new connection
			CHAT_ASSERT(m_mapClients.find(pInfo->m_hConn) == m_mapClients.end(), "m_mapClients should not have this con yet");
			//Printf("Connection request from %s", pInfo->m_info.m_szConnectionDescription);
			// A client is attempting to connect
			// Try to accept the connection.
			if (m_pInterface->AcceptConnection(pInfo->m_hConn) != k_EResultOK)
			{
				// This could fail.  If the remote host tried to connect, but then
				// disconnected, the connection may already be half closed.  Just
				// destroy whatever we have on our side.
				m_pInterface->CloseConnection(pInfo->m_hConn, 0, nullptr, false);
				//Printf("Can't accept connection.  (It was already closed?)");
				break;
			}
			// Assign the poll group
			if (!m_pInterface->SetConnectionPollGroup(pInfo->m_hConn, m_hPollGroup))
			{
				m_pInterface->CloseConnection(pInfo->m_hConn, 0, nullptr, false);
				//Printf("Failed to set poll group?");
				break;
			}
			// Generate a random nick.  A random temporary nick
			// is really dumb and not how you would write a real chat server.
			// You would want them to have some sort of signon message,
			// and you would keep their client in a state of limbo (connected,
			// but not logged on) until them.  I'm trying to keep this example
			// code really simple.
			//char nick[ 64 ];
			//sprintf(nick, "BraveWarrior%d", 10000 + (rand() % 100000));
			//// Send them a welcome message
			//sprintf(temp, "Welcome, stranger.  Thou art known to us for now as '%s'; upon thine command '/nick' we shall know thee otherwise.", nick); 
			//SendStringToClient(pInfo->m_hConn, temp); 
			//// Also send them a list of everybody who is already connected
			//if (m_mapClients.empty())
			//{
			//	SendStringToClient(pInfo->m_hConn, "Thou art utterly alone."); 
			//}
			//else
			//{
			//	sprintf(temp, "%d companions greet you:", (int)m_mapClients.size()); 
			//	for (auto &c: m_mapClients)
			//		SendStringToClient(pInfo->m_hConn, c.second.m_sNick.c_str()); 
			//}
			//// Let everybody else know who they are for now
			//sprintf(temp, "Hark!  A stranger hath joined this merry host.  For now we shall call them '%s'", nick); 
			//SendStringToAllClients(temp, pInfo->m_hConn); 
			//// Add them to the client list, using std::map wacky syntax
			m_mapClients[ pInfo->m_hConn ] = std::make_shared<ClientInfo>();
			m_mapClients[ pInfo->m_hConn ]->GetUserID() = GenerateUniqueUserID();
			//SetClientNick(pInfo->m_hConn, nick);
			break;
		}
		case k_ESteamNetworkingConnectionState_Connected:
			// We will get a callback immediately after accepting the connection.
			// Since we are the server, we can ignore this, it's not news to us.
			break;
		default:
			// Silences -Wswitch
			break;
	}
}



} /*END OF NAMESPACES*/