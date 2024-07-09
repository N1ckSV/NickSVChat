
#include "NickSV/Chat/ChatClient.h"

#include <steam/steamnetworkingsockets.h>

#include "NickSV/Chat/ChatSocket.h"

#include "NickSV/Chat/Requests/Request.h"
#include "NickSV/Chat/ClientInfo.h"

#include "NickSV/Chat/Serializers/ClientInfoRequestSerializer.h"
#include "NickSV/Chat/Serializers/MessageRequestSerializer.h"

#include <iostream>


namespace NickSV {
namespace Chat {

EResult ChatClient::Run(const ChatIPAddr &serverAddr)
{
	ChatErrorMsg errMsg;
	EResult result = OnPreRun(serverAddr, errMsg);
	if(result != EResult::Success) {
		OnRun(serverAddr, result, errMsg);
		return result; }

	result = ChatSocket::Run(serverAddr);
	if(result != EResult::Success) {
		sprintf(errMsg, "ChatSocket::Run() not succeeded");
		OnRun(serverAddr, result, errMsg);
		return result; }
	
	if (!GameNetworkingSockets_Init(nullptr, errMsg)) {
		OnRun(serverAddr, EResult::Error, errMsg);
		return EResult::Error; }

    m_pInterface = SteamNetworkingSockets();
	SteamNetworkingConfigValue_t opt;
	opt.SetPtr(k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged, (void*)SteamNetConnectionStatusChangedCallback);
    m_hConnection = m_pInterface->ConnectByIPAddress(serverAddr, 1,&opt);    
    if (m_hConnection == k_HSteamNetConnection_Invalid) {
		GameNetworkingSockets_Kill();
		sprintf(errMsg, "ISteamNetworkingSockets interface returned invalid connection");
        OnRun(serverAddr, EResult::Error, errMsg);
		return EResult::Error; }

    m_bGoingExit = false; //SHOULD BE ALWAYS FIRST BEFORE m_pConnectionThread = new ...
    m_pConnectionThread = new std::thread(&ChatClient::ConnectionThreadFunction, this);
    m_pRequestThread = new std::thread(&ChatClient::RequestThreadFunction, this);
    OnRun(serverAddr, EResult::Success, errMsg);
	return EResult::Success;
}

ChatClient::ChatClient() : ChatSocket()
{
	m_upClientInfo = MakeClientInfo();
}

ChatClient::~ChatClient() {}

void ChatClient::OnBadIncomingRequest(std::string, EResult) {};

void ChatClient::PollIncomingRequests()
{
	while (!m_bGoingExit)
	{
		ISteamNetworkingMessage *pIncomingMsg = nullptr;
		int numMsgs = m_pInterface->ReceiveMessagesOnConnection(m_hConnection, &pIncomingMsg, 1);
		if (numMsgs == 0)
			break;

		if (numMsgs < 0){
			FatalError("ISteamNetworkingSockets::ReceiveMessagesOnPollGroup returned error code");
			break;}

		std::string strReq;
		strReq.assign(static_cast<const char *>(pIncomingMsg->m_pData), pIncomingMsg->m_cbSize);
		size_t mesSize = (size_t)pIncomingMsg->m_cbSize;
		pIncomingMsg->Release();

		///
		/// FIXME unnecessary strReq assign if OnBadIncomingRequest not needed here for library user,
		/// so better think about config thingy
		///

		if(mesSize < sizeof(ERequestType)){
			OnBadIncomingRequest(std::move(strReq), EResult::InvalidRequest);
			continue;}

		if(m_handleRequestsQueue.size() >= Constant::MaxSendRequestsQueueSize){
			OnBadIncomingRequest(std::move(strReq), EResult::Overflow);
			continue;}

		m_Mutexes.handleRequestMutex.lock();
		m_handleRequestsQueue.push( { std::move(strReq), {0, 0} } );
		m_Mutexes.handleRequestMutex.unlock();
	}
}

void ChatClient::PollQueuedRequests()
{
	while (!m_bGoingExit && !m_sendRequestsQueue.empty())
	{
		m_Mutexes.sendRequestMutex.lock();
		std::string strRequest = std::move(std::get<0>(m_sendRequestsQueue.front()));
		RequestInfo rInfo 	   = std::move(std::get<1>(m_sendRequestsQueue.front()));
		m_sendRequestsQueue.pop();
		m_Mutexes.sendRequestMutex.unlock();
		auto result = SendStringToServer(strRequest);
		CHAT_ASSERT(result != EResult::InvalidParam, "The request is too big or connection is invalid");
		CHAT_EXPECT(result == EResult::Success, "Request sending not succeeded");
		if(result != EResult::Success)
			OnErrorSendingRequest(std::move(strRequest), rInfo, result);
	}
}

void ChatClient::PollConnectionChanges()
{
	s_pCallbackClientInstance = this;
	m_pInterface->RunCallbacks();
}

EResult ChatClient::SendStringToServer(const std::string& rStr)
{
	return SendStringToConnection(m_hConnection, rStr);
}

void ChatClient::ConnectionThreadFunction()
{
    ChatSocket::ConnectionThreadFunction();
	m_pInterface->CloseConnection(m_hConnection, 0, "Closing con", true);
	OnCloseSocket();
}

ClientInfo& ChatClient::GetClientInfo()
{
	return *m_upClientInfo;
}

#define CLIENT_INFO_ALLOWED_STATES(state) ((state) == EState::Active) || ((state) == EState::Unauthorized)

EResult ChatClient::HandleClientInfoRequest(ClientInfoRequest& rClientInfoRequest, RequestInfo rInfo)
{
	EResult result = OnPreHandleClientInfoRequest(rClientInfoRequest, rInfo);
	if(result != EResult::Success){
		OnHandleClientInfoRequest(rClientInfoRequest, rInfo, result);
		return result;}
	
	GetClientInfo().GetUserID() = rClientInfoRequest.GetClientInfo()->GetUserID();
	GetClientInfo().GetState()  = rClientInfoRequest.GetClientInfo()->GetState();
	CHAT_EXPECT(GetClientInfo().GetUserID() >= Constant::LibReservedUserIDs, "Invalid user id given");
	// TODO: CLIENT_INFO_ALLOWED_STATES is only for initial states, so we need to change it to virtual function
	// or allow macro redefinition 
	CHAT_EXPECT(CLIENT_INFO_ALLOWED_STATES(GetClientInfo().GetState()), "Invalid client info state given");
	if(GetClientInfo().GetUserID() < Constant::LibReservedUserIDs || !CLIENT_INFO_ALLOWED_STATES(GetClientInfo().GetState())){
		OnHandleClientInfoRequest(rClientInfoRequest, rInfo, EResult::InvalidRequest);
		return EResult::InvalidRequest;}

	if(GetClientInfo().GetState() == EState::Unauthorized)
	{
		*(rClientInfoRequest.GetClientInfo()) = GetClientInfo();
		result = QueueRequest(rClientInfoRequest, {0,0});
	}

	OnHandleClientInfoRequest(rClientInfoRequest, rInfo, result);
	return result;
}

EResult ChatClient::HandleMessageRequest(MessageRequest& rMessageRequest, RequestInfo rInfo)
{
	EResult result = OnPreHandleMessageRequest(rMessageRequest, rInfo);
	if(result != EResult::Success){
		OnHandleMessageRequest(rMessageRequest, rInfo, result);
		return result;}

	//Dunno what to add here for now TODO FIXME

	OnHandleMessageRequest(rMessageRequest, rInfo, result);
	return result;
}


ChatClient* ChatClient::s_pCallbackClientInstance = nullptr;

void ChatClient::SteamNetConnectionStatusChangedCallback(ConnectionInfo *pInfo)
{
	s_pCallbackClientInstance->OnSteamNetConnectionStatusChanged(pInfo);
}

void ChatClient::OnSteamNetConnectionStatusChanged(ConnectionInfo *pInfo)
{	
	CHAT_ASSERT(pInfo->m_hConn == m_hConnection || m_hConnection == k_HSteamNetConnection_Invalid, "");
	auto result = EResult::Success;
	switch (pInfo->m_info.m_eState)
	{
	case k_ESteamNetworkingConnectionState_None:
		// NOTE: We will get callbacks here when we destroy connections.  You can ignore these.
		break;
	case k_ESteamNetworkingConnectionState_ClosedByPeer:
	case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
	{
		m_pInterface->CloseConnection(pInfo->m_hConn, 0, nullptr, false);
		m_hConnection = k_HSteamNetConnection_Invalid;
		if(pInfo->m_info.m_eState == k_ESteamNetworkingConnectionState_ProblemDetectedLocally){
			OnConnectionStatusChanged(pInfo, EResult::Error);
			FatalError(std::string("GNS detected problem: ") + std::string(pInfo->m_info.m_szEndDebug));
			return;}
		else{
			OnConnectionStatusChanged(pInfo, EResult::Success);
			CloseSocket();
			return;}
		break;
	}

	case k_ESteamNetworkingConnectionState_Connecting:
		// We will get this callback when we start connecting.
		// We can ignore this.
		break;

	case k_ESteamNetworkingConnectionState_Connected:
		//Connected to server,
		break;

	default:
		// Silences -Wswitch
		break;
	}
	OnConnectionStatusChanged(pInfo, result);
}
	




}}  /*END OF NAMESPACES*/