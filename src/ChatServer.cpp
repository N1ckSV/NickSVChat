#include "NickSV/Chat/ChatServer.h"

#include <steam/steamnetworkingsockets.h>

#include "NickSV/Chat/ChatSocket.h"

#include "NickSV/Chat/Requests/Request.h"
#include "NickSV/Chat/ClientInfo.h"

#include "NickSV/Chat/Serializers/ClientInfoRequestSerializer.h"
#include "NickSV/Chat/Serializers/MessageRequestSerializer.h"

#include <iostream>


namespace NickSV::Chat {

EResult ChatServer::Run(const ChatIPAddr &serverAddr)
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
	opt.SetPtr(k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged,
		reinterpret_cast<void*>(SteamNetConnectionStatusChangedCallback));
	m_hListenSock = m_pInterface->CreateListenSocketIP(serverAddr, 1, &opt);
	if(m_hListenSock == k_HSteamListenSocket_Invalid){
		GameNetworkingSockets_Kill();
		sprintf(errMsg, "ISteamNetworkingSockets interface created invalid listen socket");
        OnRun(serverAddr, EResult::Error, errMsg);
		return EResult::Error; }

	m_hPollGroup = m_pInterface->CreatePollGroup();
	if(m_hPollGroup == k_HSteamNetPollGroup_Invalid) {
		GameNetworkingSockets_Kill();
		sprintf(errMsg, "ISteamNetworkingSockets interface created invalid poll group");
		OnRun(serverAddr, EResult::Error, errMsg);
        return EResult::Error; }

    m_bGoingExit = false; //SHOULD BE ALWAYS FIRST BEFORE m_pConnectionThread = new ...
    m_pConnectionThread = new std::thread(&ChatServer::ConnectionThreadFunction, this);
    m_pRequestThread = new std::thread(&ChatServer::RequestThreadFunction, this);
    OnRun(serverAddr, EResult::Success, errMsg);
	return EResult::Success;
}

ChatServer *ChatServer::s_pCallbackInstance = nullptr;
void ChatServer::SteamNetConnectionStatusChangedCallback(ConnectionInfo *pInfo)
{
	s_pCallbackInstance->OnSteamNetConnectionStatusChanged(pInfo);
}

ChatServer::ChatServer() : ChatSocket() {};

ChatServer::~ChatServer() {}

void ChatServer::ConnectionThreadFunction()
{
    ChatSocket::ConnectionThreadFunction();
	{	// lock_guard scope begin
		//LOCK_GUARD(m_Mutex.clientInfoMutex);
		for (auto& it: m_mapClients)
		{
			// Send them one more goodbye message.  Note that we also have the
			// connection close reason as a place to send final data.  However,
			// that's usually best left for more diagnostic/debug text not actual
			// protocol strings.

			//FIXME make send Request here
			std::string text = "Server is shutting down.  Goodbye.";
			SendStringToConnection(it.first, &text);

			// Close the connection.  We use "linger mode" to ask SteamNetworkingSockets
			// to flush this out and close gracefully.
			m_pInterface->CloseConnection(it.first, 0, "Server Shutdown", true);
			
			// FIXME add better cleaning here 
		}
		m_mapClients.clear();
		m_mapConnections.clear();
	}	// lock_guard scope end

	m_pInterface->CloseListenSocket(m_hListenSock);
	m_hListenSock = k_HSteamListenSocket_Invalid;

	m_pInterface->DestroyPollGroup(m_hPollGroup);
	m_hPollGroup = k_HSteamNetPollGroup_Invalid;
	OnCloseSocket();
}


void ChatServer::HandleClientInfoRequest(ClientInfoRequest* pClientInfoRequest, RequestInfo rInfo)
{
	EResult result = OnPreHandleClientInfoRequest(pClientInfoRequest, rInfo);
	if(result != EResult::Success){
		OnHandleClientInfoRequest(pClientInfoRequest, rInfo, result);
		return;}
	
	if(ValidateClientInfo(pClientInfoRequest->GetClientInfo().get()))
	{
		pClientInfoRequest->GetClientInfo()->GetState() = EState::Active;
		pClientInfoRequest->GetClientInfo()->GetUserID() = rInfo.id;
		GetClientInfo(rInfo.id) = *pClientInfoRequest->GetClientInfo();
	}
	else
		result = EResult::InvalidRequest;

	OnHandleClientInfoRequest(pClientInfoRequest, rInfo, result);
}

void ChatServer::HandleMessageRequest(MessageRequest* pMessageRequest, RequestInfo rInfo)
{
	EResult result = OnPreHandleMessageRequest(pMessageRequest, rInfo);
	if(result != EResult::Success){
		OnHandleMessageRequest(pMessageRequest, rInfo, result);
		return;}

	//Dunno what to add here for now TODO FIXME

	OnHandleMessageRequest(pMessageRequest, rInfo, result);
}


void ChatServer::PollIncomingRequests()
{
	while (!m_bGoingExit)
	{
		ISteamNetworkingMessage *pIncomingMsg = nullptr;
		ClientInfo * pClientInfo = nullptr;
		std::pair<std::string, RequestInfo> para;
    	//cppcheck-suppress variableScope
		std::string strReq;
		{	// unique_lock scope begin
			std::unique_lock<std::mutex> u_lock(m_Mutexes.clientInfoMutex);
			int numMsgs = m_pInterface->ReceiveMessagesOnPollGroup(m_hPollGroup, &pIncomingMsg, 1);
			if (numMsgs == 0)
				break;

			if (numMsgs < 0){
				u_lock.unlock();
				FatalError("ISteamNetworkingSockets::ReceiveMessagesOnPollGroup returned error code");
				break;}

			auto itClient = m_mapClients.find(pIncomingMsg->m_conn);
			CHAT_ASSERT(itClient != m_mapClients.end(), 
				"Received request, but ChatServer has no info about connection");
			pClientInfo = itClient->second.get();
		}	// unique_lock scope end
		CHAT_ASSERT(pClientInfo, "pClientInfo must not be nullptr");
		{	// ValueLockGuard scope begin
			//VALUE_LOCK_GUARD(m_UserIDLock, pClientInfo->GetUserID());
			strReq.assign(static_cast<const char*>(pIncomingMsg->m_pData), pIncomingMsg->m_cbSize);
			size_t mesSize = (size_t)pIncomingMsg->m_cbSize;
			pIncomingMsg->Release();

			///
			/// FIXME unnecessary strReq assign if OnBadIncomingRequest not needed here for api user,
			/// so better think about config thingy
			///
			if(mesSize < sizeof(ERequestType)){
				OnBadIncomingRequest(std::move(strReq), pClientInfo->GetUserID(), EResult::InvalidRequest);
				continue;}

			if (pClientInfo->GetState() == EState::Unauthorized){
				Transfer<ERequestType> type;
				std::copy(strReq.begin(), strReq.begin() + sizeof(ERequestType), type.CharArr);
				if(type.Base != ERequestType::ClientInfo){
					OnBadIncomingRequest(std::move(strReq), pClientInfo->GetUserID(), EResult::InvalidConnection);
					continue;}}

			if(m_handleRequestsQueue.size() >= Constant::MaxSendRequestsQueueSize){
				OnBadIncomingRequest(std::move(strReq), pClientInfo->GetUserID(), EResult::Overflow);
				continue;}
			
			para.first = std::move(strReq);
			para.second = { pClientInfo->GetUserID(), 0 };
		}	// ValueLockGuard scope end

		m_Mutexes.handleRequestMutex.lock();
		m_handleRequestsQueue.push(std::move(para));
		m_Mutexes.handleRequestMutex.unlock();
	}
}



void ChatServer::PollQueuedRequests()
{
	while (!m_bGoingExit && !m_sendRequestsQueue.empty()) //FIXME add max iters
	{
		std::string strRequest;
		RequestInfo rInfo;
		{	//lock guard scope begin
			LOCK_GUARD(m_Mutexes.sendRequestMutex);
			if(m_sendRequestsQueue.empty())
				break;
			strRequest = std::move(m_sendRequestsQueue.front().first);
			rInfo = m_sendRequestsQueue.front().second;
			m_sendRequestsQueue.pop();
		}	//lock guard scope end

		bool sendToSingleClient = rInfo.sendFlags & SF_SEND_TO_ONE;
		CHAT_ASSERT(rInfo.id || !sendToSingleClient, 
			"Tried to send Request to a single client but RequestInfo is given with a null user id");

		HSteamNetConnection reqCon = 0;
		bool clientFound = false;
		
		{	// ValueLockGuard scope begin
			//VALUE_LOCK_GUARD(m_UserIDLock, rInfo.id);
			{	//lock guard scope begin
				LOCK_GUARD(m_Mutexes.clientInfoMutex);
				auto iter = m_mapConnections.find(rInfo.id);
				clientFound = (iter != m_mapConnections.end());
				reqCon = (clientFound ? iter->second : 0);
			}	//lock guard scope end
			if(sendToSingleClient)
			{
				//SEND REQUEST TO RequestInfo::id
				if(!clientFound) //Not sending request to deleted client
					continue; // maybe should put OnErrorSendingRequest here
				CHAT_ASSERT(reqCon != 0, "ID used to send the request is in Reserved state (ID is not yet associated with a client)");
				auto result = SendStringToConnection(reqCon, &strRequest);
				CHAT_ASSERT(result != EResult::InvalidParam, "The request is too big or connection is invalid");
				CHAT_EXPECT(result == EResult::Success, "Request sending not succeeded");
				if(result != EResult::Success)
					OnErrorSendingRequest(std::move(strRequest), rInfo, result);
				continue;
			}
		}	// ValueLockGuard scope end

		//SEND REQUEST TO EVERYONE EXCEPT RequestInfo::id
		IDResultList list;
		
		{	//lock guard scope begin
			LOCK_GUARD(m_Mutexes.clientInfoMutex);
			for (auto &client: m_mapClients)
			{
				if (client.first != reqCon)
				{
					auto result = SendStringToConnection(client.first, &strRequest);
					CHAT_ASSERT(result != EResult::InvalidParam, "The request is too big or connection is invalid");
					if(result != EResult::Success)
						list.push_front({client.second->GetUserID(), result});

				}
			}
		}	//lock guard scope end
		if(!list.empty())
			OnErrorSendingRequestToAll(std::move(strRequest), rInfo, std::move(list));
	}
}

void ChatServer::PollConnectionChanges()
{
	s_pCallbackInstance = this;
	m_pInterface->RunCallbacks();
}

ClientInfo& ChatServer::GetClientInfo(UserID_t id)
{
	auto iter = m_mapConnections.find(id);
	ClientsMap::iterator iter2;
	if(iter == m_mapConnections.end() || (iter2 = m_mapClients.find(iter->second)) == m_mapClients.end())
		return const_cast<ClientInfo&>(InvalidClientInfo);
	return *(iter2->second);
}

EState ChatServer::GetUserIDState(UserID_t id)
{
	if(id < Constant::ApiReservedUserIDs) //Reserved by API so these are "busy"
		return EState::Busy;
	auto iter = m_mapConnections.find(id);
	if(iter == m_mapConnections.end())
		return EState::Free;
	if(iter->second == 0)
		return EState::Reserved;
	return EState::Busy;
	
}

EState ChatServer::ReserveUserID(UserID_t id)
{
	{
		LOCK_GUARD(m_Mutexes.clientInfoMutex);
		EState state = GetUserIDState(id);
		if(state == EState::Free)
			m_mapConnections[id] = 0;
		return state;
	}
}


UserID_t ChatServer::GenerateUniqueUserID()
{
	while(GetUserIDState(m_lastFreeID) != EState::Free)
	{
		CHAT_ASSERT(m_lastFreeID + 1 < ~static_cast<UserID_t>(0), "m_lastFreeID unsigned integer overflow");
		m_lastFreeID++;
	};
	return m_lastFreeID;
}

EResult ChatServer::OnPreAcceptClient(ConnectionInfo*) { return EResult::Success; };
void    ChatServer::OnAcceptClient( ConnectionInfo *, UserID_t, EResult) {};
void    ChatServer::OnBadIncomingRequest(std::string, UserID_t, EResult) {};
void 	ChatServer::OnErrorSendingRequestToAll(std::string, RequestInfo, std::forward_list<std::pair<UserID_t, EResult>>) {};


void	ChatServer::LockClient(UserID_t id) { m_UserIDLock.Lock(id); }
void	ChatServer::UnlockClient(UserID_t id) { m_UserIDLock.Unlock(id); }

EResult ChatServer::AcceptClient(ConnectionInfo *pInfo)
{
	CHAT_ASSERT(pInfo, "Invalid parameter given (pInfo == nullptr)");
    EResult result = OnPreAcceptClient(pInfo);
	if(result != EResult::Success) {
		OnAcceptClient(pInfo, 0, result);
		return result; }
	
	auto& rClientInfo = *(m_mapClients[pInfo->m_hConn] = MakeClientInfo());
	rClientInfo.GetState() = EState::Unauthorized;
	auto id = GenerateUniqueUserID();
	auto state = ReserveUserID(id); //CHAT_ASSERT DOES NOT INVOKE IN RELEASE
	CHAT_ASSERT(state == EState::Free, 
		"GenerateUniqueUserID() has to return Free id, so threads conflict maybe");
	rClientInfo.GetUserID() = id;
	m_mapConnections[id] = pInfo->m_hConn;
	OnAcceptClient(pInfo, rClientInfo.GetUserID(), EResult::Success);
	return EResult::Success;
}

EResult ChatServer::RemoveClient(HSteamNetConnection conn)
{
	auto iter = m_mapClients.find(conn);
	if(iter == m_mapClients.end())
		return EResult::NoAction;

	m_mapConnections.erase(iter->second->GetUserID());
	m_mapClients.erase(iter);
	return EResult::Success;
}


void ChatServer::OnSteamNetConnectionStatusChanged(ConnectionInfo *pInfo)
{
	auto result = EResult::Success;
	switch (pInfo->m_info.m_eState)
	{
	case k_ESteamNetworkingConnectionState_None:
		// NOTE: We will get callbacks here when we destroy connections. We can ignore these.
		break;
	case k_ESteamNetworkingConnectionState_ClosedByPeer:
	case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
	{
		CHAT_ASSERT(pInfo->m_eOldState == k_ESteamNetworkingConnectionState_Connecting , "");
		if(pInfo->m_info.m_eState == k_ESteamNetworkingConnectionState_ProblemDetectedLocally)
			result = EResult::Error;
		if (pInfo->m_eOldState == k_ESteamNetworkingConnectionState_Connected)
		{
			auto res = RemoveClient(pInfo->m_hConn); //CHAT_EXPECT DOES NOT INVOKE IN RELEASE
			CHAT_EXPECT(res == EResult::Success, "Client was connected but wasn't found to remove");
		}
		m_pInterface->CloseConnection(pInfo->m_hConn, 0, nullptr, false);
		break;
	}
	case k_ESteamNetworkingConnectionState_Connecting:
	{
		CHAT_ASSERT(m_mapClients.find(pInfo->m_hConn) == m_mapClients.end(), "m_mapClients should not have this con yet");
		result = EResult::Error;
		if( m_pInterface->AcceptConnection(pInfo->m_hConn) != k_EResultOK ) {
			m_pInterface->CloseConnection(pInfo->m_hConn, 0, nullptr, false);
			break; }
		if(!m_pInterface->SetConnectionPollGroup(pInfo->m_hConn, m_hPollGroup) ) {
			m_pInterface->CloseConnection(pInfo->m_hConn, 0, nullptr, false);
			break; }
		result = AcceptClient(pInfo);
		if( result != EResult::Success ) {
			m_pInterface->CloseConnection(pInfo->m_hConn, 0, nullptr, false);
			break; }
		result = EResult::Success;
		break;
	}
	case k_ESteamNetworkingConnectionState_Connected:
		//This case is mostly for client sockets
		break;
	default:
		// Silences -Wswitch
		break;
	}
	OnConnectionStatusChanged(pInfo, result);
}



} /*END OF NAMESPACES*/