
#include <iostream>

#include <steam/steamnetworkingsockets.h>

#include "NickSV/Chat/ChatClient.h"
#include "NickSV/Chat/ChatSocket.h"
#include "NickSV/Chat/Requests/Request.h"
#include "NickSV/Chat/ClientInfo.h"
#include "NickSV/Chat/Serializers/ClientInfoRequestSerializer.h"
#include "NickSV/Chat/Serializers/MessageRequestSerializer.h"

#include "NickSV/Tools/Utils.h"



namespace NickSV {
namespace Chat {

EResult ChatClient::Run(const ChatIPAddr &serverAddr)
{
	m_serverIPAddress = serverAddr;
	ChatErrorMsg errMsg;
	EResult result = OnPreRun(m_serverIPAddress, errMsg);
	if(!ResultIsOneOf(result, EResult::Success)) {
		OnRun(m_serverIPAddress, result, errMsg);
		return result; }

	result = ChatSocket::Run(m_serverIPAddress);
	if(!ResultIsOneOf(result, EResult::Success)) {
		sprintf(errMsg, "ChatSocket::Run() not succeeded");
		OnRun(m_serverIPAddress, result, errMsg);
		return result; }
	
	if (!GameNetworkingSockets_Init(nullptr, errMsg)) {
		OnRun(m_serverIPAddress, EResult::Error, errMsg);
		return EResult::Error; }

    m_pInterface = SteamNetworkingSockets();
	CHAT_ASSERT(m_pInterface, "GameNetworkingSockets initialized but SteamNetworkingSockets() returned nullptr");
	result = Connect(m_serverIPAddress);

	switch (result)
	{
	case EResult::Success:
		break;
	case EResult::Error:
		sprintf(errMsg, "Cannot Connect(). SteamNetworkingSockets' interface (m_pInterface) returned invalid connection (m_hConnection)");
		break;
	case EResult::TimeoutExpired:
		sprintf(errMsg, "Cannot Connect(). Timeout expired");
		break;
	default:
		sprintf(errMsg, "Cannot Connect(). LIB User defined error");
		break;
	}
	if(!ResultIsOneOf(result, EResult::Success)){
		GameNetworkingSockets_Kill();
		OnRun(m_serverIPAddress, result, errMsg);
		return result; }


	
    m_bGoingExit = false; //SHOULD BE ALWAYS FIRST BEFORE m_pConnectionThread = new ...
    m_pConnectionThread = new std::thread(&ChatClient::ConnectionThreadFunction, this);
    m_pRequestThread = new std::thread(&ChatClient::RequestThreadFunction, this);
    OnRun(m_serverIPAddress, EResult::Success, errMsg);
	return EResult::Success;
}

EResult ChatClient::Connect(ChatIPAddr &serverAddr, std::chrono::milliseconds timeout, unsigned int maxAttempts)
{
	using namespace std::chrono;
	EResult result = OnPreConnect(serverAddr, timeout, maxAttempts);
	if(result != EResult::Success) {
		OnConnect(serverAddr, result, milliseconds::zero(), 0);
		return result; }
	
	if((maxAttempts == 0) || (timeout.count() < 500) || (static_cast<unsigned int>(timeout.count()) < maxAttempts*500)){
		OnConnect(serverAddr, EResult::InvalidParam, milliseconds::zero(), 0);
		return  EResult::InvalidParam; }
	
	CHAT_ASSERT(duration_cast<milliseconds>(timeout / maxAttempts).count(), "Something went wrong, step should be at least 1 ms");
	microseconds step = duration_cast<microseconds>(timeout / maxAttempts);

	unsigned int countRetries = 0;
	microseconds connectTime {0};
	result = EResult::TimeoutExpired;
	CHAT_ASSERT(m_pInterface, "Connect() called but GNS interface isn't initialized");
	m_pInterface->CloseConnection(m_hConnection, 0, nullptr, false);
	m_hConnection = k_HSteamNetConnection_Invalid;

	constexpr static size_t optionsArraySize = 2;
	SteamNetworkingConfigValue_t aOptions[optionsArraySize];
	aOptions[0].SetInt32( k_ESteamNetworkingConfig_TimeoutConnected, 25 );
	aOptions[1].SetPtr(k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged, reinterpret_cast<void*>(SteamNetConnectionStatusChangedCallback));
	SteamNetConnectionInfo_t info;

	while((connectTime < duration_cast<microseconds>(timeout)) && (countRetries < maxAttempts))
	{
		auto timePointBeforeAttempt = high_resolution_clock::now();
		++countRetries;
    	m_hConnection = m_pInterface->ConnectByIPAddress(serverAddr, optionsArraySize, aOptions);
		if(m_hConnection != k_HSteamNetConnection_Invalid)
		{
        	m_pInterface->GetConnectionInfo(m_hConnection, &info);
			if(info.m_eState == k_ESteamNetworkingConnectionState_Connected){
				result = EResult::Success;
				break;}
			if(info.m_eState == k_ESteamNetworkingConnectionState_Connecting)
			{
				std::this_thread::sleep_for((step*(maxAttempts + countRetries))/(2*maxAttempts));
        		m_pInterface->GetConnectionInfo(m_hConnection, &info);
			}
			if(info.m_eState == k_ESteamNetworkingConnectionState_Connected){
				result = EResult::Success;
				break;}
			m_pInterface->CloseConnection(m_hConnection, 0, nullptr, false);
			m_hConnection = k_HSteamNetConnection_Invalid;
		}
		microseconds diff = duration_cast<microseconds>(high_resolution_clock::now() - timePointBeforeAttempt);
		OnConnectAttemptFailed(serverAddr, duration_cast<milliseconds>(connectTime + diff), countRetries, maxAttempts);
		diff = duration_cast<microseconds>(high_resolution_clock::now() - timePointBeforeAttempt);
		if(diff < step)
			std::this_thread::sleep_for(step - diff);
		connectTime += duration_cast<microseconds>(high_resolution_clock::now() - timePointBeforeAttempt);
	}
	if(m_hConnection == k_HSteamNetConnection_Invalid)
		result = EResult::Error;
	
	OnConnect(serverAddr, result, duration_cast<milliseconds>(connectTime), countRetries);
	return result;
}

ChatClient::ChatClient()
 	: m_upClientInfo(Tools::MakeUnique<ClientInfo>())
{
	s_pCallbackClientInstance = this;
}


ChatClient::~ChatClient() {}


EResult ChatClient::OnPreHandleRequest(ClientInfoRequest&, RequestInfo&)		  		    { return EResult::Success; };
EResult ChatClient::OnPreHandleRequest(MessageRequest&,    RequestInfo&) 		  			{ return EResult::Success; };
EResult ChatClient::OnPreConnect(ChatIPAddr &, std::chrono::milliseconds &, unsigned int &) { return EResult::Success; };

void 	ChatClient::OnBadIncomingRequest(std::string, EResult) 									   					  {};
void    ChatClient::OnHandleRequest(const ClientInfoRequest&, RequestInfo, EResult, TaskInfo)	   					  {};
void    ChatClient::OnHandleRequest(const MessageRequest&, RequestInfo, EResult)				   					  {};
void    ChatClient::OnConnect(const ChatIPAddr &, EResult, std::chrono::milliseconds , unsigned int)				  {};
void    ChatClient::OnConnectAttemptFailed(ChatIPAddr &, std::chrono::milliseconds, unsigned int, unsigned int)	  	  {};

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
		strReq.assign(static_cast<const char *>(pIncomingMsg->m_pData), static_cast<size_t>(pIncomingMsg->m_cbSize));
		size_t mesSize = static_cast<size_t>(pIncomingMsg->m_cbSize);
		pIncomingMsg->Release();

		///
		/// TODO FIXME unnecessary strReq assign if OnBadIncomingRequest not needed here for library user,
		/// so better think about config thingy
		///

		if(mesSize < sizeof(ERequestType)){
			OnBadIncomingRequest(std::move(strReq), EResult::InvalidRequest);
			continue;}

		if(m_handleRequestsQueue.Size() >= Constant::MaxSendRequestsQueueSize){
			OnBadIncomingRequest(std::move(strReq), EResult::Overflow);
			continue;}

		m_handleRequestsQueue.Push( { std::move(strReq), {0, 0} } );
	}
}

void ChatClient::PollQueuedRequests()
{
	while (!m_bGoingExit && !m_sendRequestsQueue.IsEmpty()) //FIXME TODO ADD TIME LIMIT HERE
	{
		auto tuple = m_sendRequestsQueue.Pop();
		auto strRequest = std::move(std::get<0>(tuple));
		auto rInfo 	   	= std::move(std::get<1>(tuple));
		auto promise 	= std::move(std::get<2>(tuple));
		auto result = SendStringToServer(strRequest);
		promise.set_value(result);
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
	m_pInterface->CloseConnection(m_hConnection, 0, "Closing con", true); //FIXME TODO: mb better set nReason and pszDebug
	m_hConnection = k_HSteamNetConnection_Invalid;
	OnCloseSocket();
}

ClientInfo& ChatClient::GetClientInfo()
{
	return *m_upClientInfo;
}

#define CLIENT_INFO_ALLOWED_STATES(state) (((state) == EState::Active) || ((state) == EState::Unauthorized))
#define NO_ACTION_TASK_INFO TaskInfo{ EResult::NoAction, Tools::MakeReadyFuture(EResult::NoAction)}

EResult ChatClient::HandleRequest(ClientInfoRequest& rClientInfoRequest, RequestInfo rInfo)
{
	EResult result = OnPreHandleRequest(rClientInfoRequest, rInfo);
	if(result != EResult::Success){
		OnHandleRequest(rClientInfoRequest, rInfo, result, NO_ACTION_TASK_INFO );
		return result;}
	
	auto& info = rClientInfoRequest.GetClientInfo();
	CHAT_EXPECT(!IsLibReservedID(info.GetUserID()), "Invalid user id given");
	// FIXME TODO: CLIENT_INFO_ALLOWED_STATES is only for initial states, so we need to change it to virtual function
	// or allow macro redefinition 
	CHAT_EXPECT(CLIENT_INFO_ALLOWED_STATES(GetClientInfo().GetState()), "Invalid client info state given");
	if(IsLibReservedID(info.GetUserID()) || !CLIENT_INFO_ALLOWED_STATES(info.GetState())){
		OnHandleRequest(rClientInfoRequest, rInfo, EResult::InvalidRequest,  NO_ACTION_TASK_INFO);
		return EResult::InvalidRequest;}

	bool serverMadeUsActive = (info.GetState() == EState::Active);
	bool weNotYetActive = (GetClientInfo().GetState() != EState::Active);

	if(weNotYetActive && serverMadeUsActive)
		GetClientInfo().GetState() = EState::Active;

	CHAT_EXPECT(!(serverMadeUsActive && weNotYetActive && (GetClientInfo() != info)), //FIXME TODO. Not really sure we should'n expect that
		"Got ClientInfoRequest with Active state, but ClientInfos is different on our and server side");

	if(!serverMadeUsActive)
	{
		GetClientInfo().GetState() = info.GetState();
		GetClientInfo().GetUserID() = info.GetUserID();
		info = GetClientInfo();
		auto taskInfo = QueueRequest(rClientInfoRequest);
		OnHandleRequest(rClientInfoRequest, rInfo, result, std::move(taskInfo));
		return result;
	}

	OnHandleRequest(rClientInfoRequest, rInfo, result, NO_ACTION_TASK_INFO);
	return result;
}

EResult ChatClient::HandleRequest(MessageRequest& rMessageRequest, RequestInfo rInfo)
{
	EResult result = OnPreHandleRequest(rMessageRequest, rInfo);
	if(result != EResult::Success){
		OnHandleRequest(rMessageRequest, rInfo, result);
		return result;}
 
	//TODO FIXME Dunno what to add here for now

	OnHandleRequest(rMessageRequest, rInfo, result);
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
		m_pInterface->CloseConnection(pInfo->m_hConn, 0, nullptr, false);  //FIXME TODO: mb better set nReason and pszDebug
		m_hConnection = k_HSteamNetConnection_Invalid;
		OnConnectionStatusChanged(pInfo, EResult::ClosedConnection);
		CloseSocket();
		return;
	case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
		m_pInterface->CloseConnection(pInfo->m_hConn, 0, nullptr, false);  //FIXME TODO: mb better set nReason and pszDebug
		m_hConnection = k_HSteamNetConnection_Invalid;
		result = Connect(m_serverIPAddress);
		if(result == EResult::Success)
			break;
		m_hConnection = k_HSteamNetConnection_Invalid;
		OnConnectionStatusChanged(pInfo, result);
		FatalError(std::string("GNS detected problem: ") + std::string(pInfo->m_info.m_szEndDebug));
		return;
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