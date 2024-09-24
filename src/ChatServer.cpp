

#include <iostream>

#include <steam/steamnetworkingsockets.h>

#include "NickSV/Tools/Memory.h"

#include "NickSV/Chat/ChatServer.h"
#include "NickSV/Chat/ChatSocket.h"
#include "NickSV/Chat/Utils.h"
#include "NickSV/Chat/Request.h"
#include "NickSV/Chat/ClientInfo.h"
#include "NickSV/Chat/Message.h"

namespace NickSV {
namespace Chat {

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

ChatServer* ChatServer::s_pCallbackServerInstance = nullptr;

void ChatServer::SteamNetConnectionStatusChangedCallback(ConnectionInfo *pInfo)
{
	s_pCallbackServerInstance->OnSteamNetConnectionStatusChanged(pInfo);
}

ChatServer::ChatServer() : ChatSocket()
{
	s_pCallbackServerInstance = this;
};

ChatServer::~ChatServer() {}

void ChatServer::ConnectionThreadFunction()
{
    ChatSocket::ConnectionThreadFunction();
	{	// ClientLockAllGuard scope begin
		ClientLockAllGuard lock_g(m_ClientLock);
		for (auto& it: m_mapClients)
		{
			//FIXME make send Request here
			std::string text = "Server is shutting down.  Goodbye.";
			SendStringToConnection(it.second.Connection, text);

			// Close the connection.  We use "linger mode" to ask SteamNetworkingSockets
			// to flush this out and close gracefully.
			m_pInterface->CloseConnection(it.second.Connection, 0, "Server Shutdown", true);

			// FIXME add better cleaning here 
		}
		m_mapClients.clear();
		m_mapConnections.clear();
	} 	// ClientLockAllGuard scope end

	m_pInterface->CloseListenSocket(m_hListenSock);
	m_hListenSock = k_HSteamListenSocket_Invalid;

	m_pInterface->DestroyPollGroup(m_hPollGroup);
	m_hPollGroup = k_HSteamNetPollGroup_Invalid;
	GameNetworkingSockets_Kill();
	OnCloseSocket();
}

#define NO_ACTION_TASK_INFO TaskInfo{ EResult::NoAction, Tools::MakeReadyFuture(EResult::NoAction)}

EResult ChatServer::HandleRequest(ClientInfoRequest& rReq, RequestInfo rInfo)
{
	CHAT_ASSERT(rReq.Type() == ERequestType::ClientInfo, "ClientInfoRequest has wrong (not ERequestType::ClientInfo) type");
	CHAT_ASSERT(rReq.HasClientInfo(), "ClientInfo Request has no ClientInfo");

	ClientInfo clientInfo;
	bool unpackResult = rReq.UnpackClientInfoTo(&clientInfo);
	CHAT_ASSERT(unpackResult, "Got ClientInfoRequest, but failed to unpack ClientInfo");
	
	bool wasSameIDs = (clientInfo.UserID() == rInfo.userID);
	EResult result = OnPreHandleRequest(rReq, rInfo);
	//-> EResult::Success 		 - nothing changed, at least default data in ClientInfo and RequestInfo hasnt been changed 
	//   (RequestInfo::extraInfo is allowed to be changed)
	//-> EResult::InvalidRequest - bad request, we need to resend authentication
	//-> Anything else  		 - we should do nothing
	if(!ResultIsOneOf(result, EResult::Success, EResult::InvalidRequest)){
		 OnHandleRequest(rReq, rInfo, const_cast<ClientInfo&>(ClientInfo::DefaultInstance()), result, NO_ACTION_TASK_INFO);
		return result;}

	bool nowIsNotSameIDs = (clientInfo.UserID()  != rInfo.userID);

	if(IsLibReservedID(clientInfo.UserID()) || IsLibReservedID(rInfo.userID) || (wasSameIDs && nowIsNotSameIDs) ){
		 OnHandleRequest(rReq, rInfo, const_cast<ClientInfo&>(ClientInfo::DefaultInstance()), EResult::Error, NO_ACTION_TASK_INFO);
		return EResult::Error;}
	
	if(nowIsNotSameIDs)
	{
		clientInfo.SetUserID(rInfo.userID);
		result = EResult::InvalidRequest;
	}

	clientInfo.SetState(result == EResult::Success ? EState::Active : EState::Unauthorized);

	ClientLockGuard lock_g(m_ClientLock, rInfo.userID);
	auto& refClientInfo = GetClientInfo(rInfo.userID);
	if(&refClientInfo == &ClientInfo::DefaultInstance()){
		 OnHandleRequest(rReq, rInfo, refClientInfo, EResult::InvalidConnection, NO_ACTION_TASK_INFO);
		return EResult::InvalidConnection;}

	if(result == EResult::Success)
		refClientInfo = std::move(clientInfo);

	rReq.PackClientInfoFrom(refClientInfo);
	rInfo.sendFlags = SF_SEND_TO_ONE; 
	TaskInfo taskInfo = QueueRequest(rReq, rInfo);
	 OnHandleRequest(rReq, rInfo, refClientInfo, result, std::move(taskInfo));
	return result;
}


EResult ChatServer::HandleRequest(MessageRequest& rReq, RequestInfo rInfo)
{
	CHAT_ASSERT(rReq.Type() == ERequestType::Message, "MessageRequest has wrong (not ERequestType::Message) type");
	CHAT_ASSERT(rReq.HasMessage(), "Message Request has no Message");

	auto& clientInfo = GetClientInfo(rInfo.userID);
	if(&clientInfo == &ClientInfo::DefaultInstance())
		 OnHandleRequest(rReq, rInfo, EResult::InvalidConnection, NO_ACTION_TASK_INFO);

	if(clientInfo.State() == EState::Unauthorized)
		 OnHandleRequest(rReq, rInfo, EResult::InvalidRequest, NO_ACTION_TASK_INFO);

	Message message;
	bool unpackResult = rReq.UnpackMessageTo(&message);
	CHAT_ASSERT(unpackResult, "Got MessageRequest, but failed to unpack Message");
	message.SetSenderID(rInfo.userID);

	rReq.PackMessageFrom(message);
	EResult result = OnPreHandleRequest(rReq, rInfo);
	if(result != EResult::Success){
		 OnHandleRequest(rReq, rInfo, result, NO_ACTION_TASK_INFO);
		return result;}

	rInfo.sendFlags = SF_SEND_TO_ALL | SF_SEND_TO_ACTIVE;
	TaskInfo taskInfo = QueueRequest(rReq, rInfo);
	OnHandleRequest(rReq, rInfo, result, std::move(taskInfo));
	return result;
}


void ChatServer::PollIncomingRequests()
{
	while_limit(!m_bGoingExit, 30) 
	{
		ISteamNetworkingMessage *pIncomingMsg = nullptr;
    	//cppcheck-suppress variableScope
		std::string strReq;
		
		m_ClientLock.Lock(0); // We dont know id yet, 
							  // and this one is supposed to 
							  // lock every method that is 
							  // trying to add/remove clients
		UniqueUnlocker ZeroUnlocker(&m_ClientLock, ClientLock_t::Unlocker(0));					
		int numMsgs = m_pInterface->ReceiveMessagesOnPollGroup(m_hPollGroup, &pIncomingMsg, 1);
		if (numMsgs == 0)
			break;

		if (numMsgs < 0){
			FatalError("ISteamNetworkingSockets::ReceiveMessagesOnPollGroup returned error code");
			break;}
		
		auto& rClient = GetClientInfoByConnection(pIncomingMsg->m_conn);
		
		CHAT_ASSERT(&rClient != &ClientInfo::DefaultInstance(), 
			"Received request, but ChatServer has no info about connection");
		CHAT_ASSERT(rClient.UserID(), 
			"Received request, but ChatServer has invalid info about connection");
		m_ClientLock.Lock(rClient.UserID());
		ZeroUnlocker.reset();
		UniqueUnlocker unlocker(&m_ClientLock, ClientLock_t::Unlocker(rClient.UserID()));

		size_t mesSize = static_cast<size_t>(pIncomingMsg->m_cbSize);
		strReq.assign(static_cast<const char*>(pIncomingMsg->m_pData), mesSize);
		pIncomingMsg->Release();

		///
		/// FIXME unnecessary strReq assign if OnBadIncomingRequest not needed here for library user,
		/// so better think about config thingy
		///
		
		if(mesSize < sizeof(ERequestType)){ 
			OnBadIncomingRequest(std::move(strReq), rClient, EResult::InvalidRequest);
			continue;}
			
		if(m_handleRequestsQueue.Size() >= Constant::MaxSendRequestsQueueSize){
			OnBadIncomingRequest(std::move(strReq), rClient, EResult::Overflow);
			continue;}

		m_handleRequestsQueue.Push( { std::move(strReq), RequestInfo{ rClient.UserID(), SF_SEND_TO_ONE } } );
	}
}



void ChatServer::PollQueuedRequests()
{
	while_limit(!m_bGoingExit && !m_sendRequestsQueue.IsEmpty(), 30)
	{
		auto tuple = m_sendRequestsQueue.Pop();
		auto strRequest 	    = std::move(std::get<0>(tuple));
		auto rInfo 	   		    = std::move(std::get<1>(tuple));
		auto sendRequestPromise = std::move(std::get<2>(tuple));
		EResult result = EResult::Success;
		auto setter = [&result](std::promise<NickSV::Chat::EResult>* pPromise)
			{
				pPromise->set_value(result);
			};
		std::unique_ptr<std::promise<NickSV::Chat::EResult>, decltype(setter)> setPromiseRAII
			(&sendRequestPromise, setter);
		bool sendToAllClients = static_cast<bool>(rInfo.sendFlags & SF_SEND_TO_ALL);
		CHAT_ASSERT(rInfo.userID || sendToAllClients, 
			"Tried to send Request to a single client but RequestInfo is given with a null user id");
		HSteamNetConnection reqCon = k_HSteamNetConnection_Invalid;
		m_ClientLock.Lock(rInfo.userID); //CHANGE THIS TWO LINES TO A SINGLE UNIQUE LOCK
		UniqueUnlocker unlocker(&m_ClientLock, ClientLock_t::Unlocker(rInfo.userID));
		auto iter = m_mapClients.find(rInfo.userID);
		bool clientFound = (iter != m_mapClients.end());
		if(clientFound)
			reqCon = iter->second.Connection;
		bool isToActiveOnly = static_cast<bool>(rInfo.sendFlags & SF_SEND_TO_ACTIVE);
		if(!sendToAllClients)
		{
			//SEND REQUEST TO RequestInfo::id
			if(!clientFound) //Not sending request to deleted client
			{
				result = EResult::ClosedConnection;
				continue; // maybe should put OnErrorSendingRequest here
			}
			CHAT_ASSERT(reqCon != k_HSteamNetConnection_Invalid, "ID used to send the request is in Reserved state (ID is not yet associated with a client)");
			if (isToActiveOnly && iter->second.clientInfo.State() != EState::Active)
			{
				result = EResult::InvalidParam;
				setPromiseRAII.reset();
				OnErrorSendingRequest(std::move(strRequest), rInfo, EResult::InvalidParam);
				continue;
			}
			result = SendStringToConnection(reqCon, strRequest);
			setPromiseRAII.reset();
			CHAT_ASSERT(result != EResult::InvalidParam, "The request is too big or connection is invalid");
			CHAT_EXPECT(result == EResult::Success, "Request sending not succeeded");
			if(result != EResult::Success)
				OnErrorSendingRequest(std::move(strRequest), rInfo, result);
			continue;
		}
		unlocker.reset();

		//SEND REQUEST TO EVERYONE EXCEPT RequestInfo::id

		m_ClientLock.Lock(0); // Locking one unused value so no thread can lock all values
		unlocker = UniqueUnlocker(&m_ClientLock, ClientLock_t::Unlocker(0));
		IDResultList_t list;
		for (auto &item: m_mapClients)
		{
			ClientLockGuard lock_g(m_ClientLock, item.first);
			auto& client = item.second;
			if (client.Connection == reqCon || (isToActiveOnly && client.clientInfo.State() != EState::Active))
				continue;
			result = SendStringToConnection(client.Connection, strRequest);
			CHAT_ASSERT(result != EResult::InvalidParam, "The request is too big or connection is invalid");
			if(result != EResult::Success)
				list.push_front({client.clientInfo.UserID(), result});
		}
		unlocker.reset();
		if(!list.empty())
		{
			result = EResult::Error;
			setPromiseRAII.reset();
			OnErrorSendingRequestToAll(std::move(strRequest), rInfo, std::move(list));
			continue;
		}	
	}
}

void ChatServer::PollConnectionChanges()
{
	s_pCallbackServerInstance = this;
	m_pInterface->RunCallbacks();
}


ClientInfo& ChatServer::GetClientInfo(UserID_t id)
{
	auto iter = m_mapClients.find(id);
	if(iter == m_mapClients.end())
		return const_cast<ClientInfo&>(ClientInfo::DefaultInstance());
	return iter->second.clientInfo;
}

ClientInfo& ChatServer::GetClientInfoByConnection(HSteamNetConnection conn)
{
	if(conn == k_HSteamNetConnection_Invalid)
		return const_cast<ClientInfo&>(ClientInfo::DefaultInstance());
	auto iter = m_mapConnections.find(conn);
	if(iter == m_mapConnections.end())
		return const_cast<ClientInfo&>(ClientInfo::DefaultInstance());
	auto iter2 = m_mapClients.find(iter->second);
	if(iter2 == m_mapClients.end())
		return const_cast<ClientInfo&>(ClientInfo::DefaultInstance());
	return iter2->second.clientInfo;
}

EState ChatServer::GetUserIDState(UserID_t id)
{
	if(IsLibReservedID(id)) //Reserved by library so these are "busy"
		return EState::Busy;
	auto iter = m_mapClients.find(id);
	if (iter == m_mapClients.end())
		return EState::Free;
	if (iter->second.Connection == k_HSteamNetConnection_Invalid)
		return EState::Reserved;
	return EState::Busy;
	
}

EState ChatServer::ReserveUserID(UserID_t id)
{
	EState state = GetUserIDState(id);
	if(state == EState::Free)
		m_mapClients[id].Connection = k_HSteamNetConnection_Invalid;
	return state;
}

EResult ChatServer::RemoveClient(UserID_t id)
{
	if (IsLibReservedID(id))
		return EResult::InvalidParam;

	ClientLockAllGuard lock_g(m_ClientLock);
	auto iterCl = m_mapClients.find(id);
	if (iterCl == m_mapClients.end())
	{
		#ifndef NDEBUG
		for (auto conn_pair : m_mapConnections)
		{
			CHAT_ASSERT(conn_pair.second != id,
			"Client with given ID not found in m_mapClients, but ID is found in m_mapConnections");
		}
		#endif
		return EResult::NoAction;
	}
	if(iterCl->second.Connection != k_HSteamNetConnection_Invalid)
	{
		auto iterCon = m_mapConnections.find(iterCl->second.Connection);
		CHAT_ASSERT(iterCon != m_mapConnections.end(), 
		"Client found in m_mapClients with valid HSteamNetConnection, but not found in m_mapConnections");
		CHAT_ASSERT(iterCon->second == id, 
			R"(Client found in m_mapConnections and m_mapClients,
			 but given ID is different from the one in m_mapConnections)");
		m_pInterface->CloseConnection(iterCl->second.Connection, 0, nullptr, false);
		m_mapConnections.erase(iterCon);
	}
	m_mapClients.erase(iterCl);
	return EResult::Success;
}

EResult ChatServer::RemoveClientByConnection(HSteamNetConnection conn)
{
	if (conn == k_HSteamNetConnection_Invalid)
		return EResult::InvalidParam;

	ClientLockAllGuard lock_g(m_ClientLock);

	auto iterCon = m_mapConnections.find(conn);
	if (iterCon != m_mapConnections.end())
	{
		auto iterCl = m_mapClients.find(iterCon->second);
		CHAT_ASSERT(iterCl != m_mapClients.end(), 
			"Client found in m_mapConnections, but not found in m_mapClients");
		CHAT_ASSERT(iterCl->second.Connection == conn,
			R"(Client found in m_mapConnections and m_mapClients,
			 but given HSteamNetConnection is different from the one in m_mapClients)");
		m_pInterface->CloseConnection(conn, 0, nullptr, false);
		m_mapConnections.erase(iterCon);
		m_mapClients.erase(iterCl);
		return EResult::Success;
	}
	else
	{
		#ifndef NDEBUG
		for (const auto& item : m_mapClients)
		{
			CHAT_ASSERT(item.second.Connection != conn,
			R"("Client with given HSteamNetConnection not found in m_mapConnections,
			 but HSteamNetConnection is found in m_mapClients)");
		}
		#endif
	}
	if(m_pInterface->CloseConnection(conn, 0, nullptr, false))
		return EResult::ClosedConnection;

	return EResult::NoAction;
}

UserID_t ChatServer::GenerateUniqueUserID()
{
	//FIXME TODO this function needs some scope lock, because locking all ids externally is too heavy
	while(GetUserIDState(m_lastFreeID) != EState::Free)
	{
		CHAT_ASSERT(m_lastFreeID + 1 < ~static_cast<UserID_t>(0), "m_lastFreeID unsigned integer overflow");
		++m_lastFreeID;
	};
	return m_lastFreeID++;
}

EResult ChatServer::OnPreAcceptClient(const ConnectionInfo&, RequestInfo&) 				  		{ return EResult::Success; };
EResult ChatServer::OnPreHandleRequest(ClientInfoRequest&, RequestInfo&) 						{ return EResult::Success; };
EResult ChatServer::OnPreHandleRequest(MessageRequest&, RequestInfo&) 							{ return EResult::Success; };

void    ChatServer::OnBadIncomingRequest(std::string, ClientInfo&, EResult) 	 										  {};
void    ChatServer::OnAcceptClient( const ConnectionInfo&, const RequestInfo&, ClientInfo&, EResult, TaskInfo)  	 	  {};
void    ChatServer::OnHandleRequest(const ClientInfoRequest&, RequestInfo, ClientInfo&, EResult, TaskInfo)				  {};
void    ChatServer::OnHandleRequest(const MessageRequest&   , RequestInfo, EResult, TaskInfo) 	      				      {};
void 	ChatServer::OnErrorSendingRequestToAll(std::string, RequestInfo, std::forward_list<std::pair<UserID_t, EResult>>) {};

EResult ChatServer::AcceptClient(ConnectionInfo& rConInfo)
{
	
	RequestInfo rInfo = {GenerateUniqueUserID(), SF_SEND_TO_ONE};
	UserID_t &id = rInfo.userID;
	m_ClientLock.Lock(id);
	auto state = ReserveUserID(id); //CHAT_ASSERT DOES NOT INVOKE IN RELEASE
	CHAT_ASSERT(state == EState::Free, 
		"GenerateUniqueUserID() has to return Free id, so threads conflict maybe");
	m_ClientLock.Unlock(id);

    EResult result = OnPreAcceptClient(rConInfo, rInfo);
	if(result != EResult::Success) {
		//FIXME TODO rInfo's userID now can be cleared and unreserved to be in GenerateUniqueUserID() method again (m_lastFreeID)
		OnAcceptClient(rConInfo, rInfo, const_cast<ClientInfo&>(ClientInfo::DefaultInstance()), result, NO_ACTION_TASK_INFO);
		return result; } 
	
	CHAT_ASSERT(&GetClientInfoByConnection(rConInfo.m_hConn) == &ClientInfo::DefaultInstance(), 
		"Trying to accept client with HSteamNetConnection that already in m_mapConnections or m_mapClients");
	m_ClientLock.LockAll();
	m_mapClients[id];
	auto& rClientInfo = m_mapClients[id].clientInfo;
	m_mapConnections[rConInfo.m_hConn] = id;
	m_ClientLock.UnlockAll(id);
	UniqueUnlocker unlocker(&m_ClientLock, ClientLock_t::Unlocker(id));
	rClientInfo.SetState(EState::Unauthorized);
	rClientInfo.SetUserID(id);
	m_mapClients.at(id).Connection = rConInfo.m_hConn;

	ClientInfoRequest request;
	request.PackClientInfoFrom(rClientInfo);
	auto taskInfo = QueueRequest(request, rInfo);

	OnAcceptClient(rConInfo, rInfo, rClientInfo, result, std::move(taskInfo));
	return result;
}

ChatServer::ClientLock_t& ChatServer::GetClientLock()
{ return m_ClientLock; }


EResult ChatServer::ChangeClientID(UserID_t currentID, UserID_t newID)
{
	if(currentID == newID)
		return EResult::NoAction;
	
	auto oldState = GetUserIDState(currentID);
	if(oldState == EState::Free || ReserveUserID(newID) != EState::Free)
		return EResult::InvalidParam;

	auto& client = m_mapClients.at(newID);
	std::swap(m_mapClients.at(currentID), client);
	m_mapClients.erase(currentID);
	client.clientInfo.SetUserID(newID);
	if(oldState == EState::Busy)
		m_mapConnections.at(client.Connection) = newID;
		
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
		if (pInfo->m_eOldState == k_ESteamNetworkingConnectionState_Connected)
		{
			result = RemoveClientByConnection(pInfo->m_hConn); //CHAT_EXPECT DOES NOT INVOKE IN RELEASE
			CHAT_EXPECT(result == EResult::Success, "Client was connected but wasn't found to remove");
		}
		else
		{
			CHAT_ASSERT(pInfo->m_eOldState == k_ESteamNetworkingConnectionState_Connecting , "Unresolved Connection state");
			m_pInterface->CloseConnection(pInfo->m_hConn, 0, nullptr, false);
			result = EResult::ClosedConnection;
		}
		break;
	}
	case k_ESteamNetworkingConnectionState_Connecting:
	{
		CHAT_ASSERT(m_mapClients.find(pInfo->m_hConn) == m_mapClients.end(), "m_mapClients should not have this con yet");
		result = EResult::InvalidConnection;
		if(m_pInterface->AcceptConnection(pInfo->m_hConn) != k_EResultOK ||
		   	!m_pInterface->SetConnectionPollGroup(pInfo->m_hConn, m_hPollGroup) ||
				//cppcheck-suppress redundantAssignment
		   		!ResultIsOneOf(result = AcceptClient(*pInfo), EResult::Success))
					m_pInterface->CloseConnection(pInfo->m_hConn, 0, nullptr, false);
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




ChatServer::ClientsIterator::ClientsIterator(ClientsMap_t::iterator it) : it_(it) {}

ChatServer::ClientsIterator::reference ChatServer::ClientsIterator::operator*() const { return it_->second.clientInfo; }

ChatServer::ClientsIterator::pointer ChatServer::ClientsIterator::operator->() const { return &(it_->second.clientInfo); }

ChatServer::ClientsIterator& ChatServer::ClientsIterator::operator++() { ++it_; return *this; }

ChatServer::ClientsIterator ChatServer::ClientsIterator::operator++(int) { ClientsIterator tmp = *this; ++(*this); return tmp; }

bool operator==(const ChatServer::ClientsIterator& a, const ChatServer::ClientsIterator& b) { return a.it_ == b.it_; }
bool operator!=(const ChatServer::ClientsIterator& a, const ChatServer::ClientsIterator& b) { return !(a == b); }




ChatServer::ConstClientsIterator::ConstClientsIterator(ClientsMap_t::const_iterator it) : it_(it) {}

ChatServer::ConstClientsIterator::reference ChatServer::ConstClientsIterator::operator*() const { return it_->second.clientInfo; }

ChatServer::ConstClientsIterator::pointer ChatServer::ConstClientsIterator::operator->() const { return &(it_->second.clientInfo); }

ChatServer::ConstClientsIterator& ChatServer::ConstClientsIterator::operator++() { ++it_; return *this; }

ChatServer::ConstClientsIterator ChatServer::ConstClientsIterator::operator++(int) { ConstClientsIterator tmp = *this; ++(*this); return tmp; }

bool operator==(const ChatServer::ConstClientsIterator& a, const ChatServer::ConstClientsIterator& b) { return a.it_ == b.it_; }
bool operator!=(const ChatServer::ConstClientsIterator& a, const ChatServer::ConstClientsIterator& b) { return !(a == b); }




ChatServer::Clients ChatServer::GetClients() { return Clients(&m_mapClients); };

ChatServer::Clients::Clients(ClientsMap_t* const _pClientsMap) : pClientsMap(_pClientsMap) {};
    
ClientInfo& ChatServer::Clients::Get(UserID_t id) { return pClientsMap->at(id).clientInfo; }
const ClientInfo& ChatServer::Clients::Get(UserID_t id) const { return pClientsMap->at(id).clientInfo; }

ClientInfo& ChatServer::Clients::At(UserID_t id) { return pClientsMap->at(id).clientInfo; }
const ClientInfo& ChatServer::Clients::At(UserID_t id) const { return pClientsMap->at(id).clientInfo; }

ClientInfo& ChatServer::Clients::operator[](UserID_t id) { return pClientsMap->at(id).clientInfo; }
const ClientInfo& ChatServer::Clients::operator[](UserID_t id) const { return pClientsMap->at(id).clientInfo; }

ChatServer::ClientsIterator ChatServer::Clients::begin() { return ClientsIterator(pClientsMap->begin()); }
ChatServer::ClientsIterator ChatServer::Clients::end()   { return ClientsIterator(pClientsMap->end()  ); }

ChatServer::ConstClientsIterator ChatServer::Clients::begin() const { return ConstClientsIterator(pClientsMap->cbegin()); }
ChatServer::ConstClientsIterator ChatServer::Clients::end()   const { return ConstClientsIterator(pClientsMap->cend()  ); }

ChatServer::ConstClientsIterator ChatServer::Clients::cbegin() const { return ConstClientsIterator(pClientsMap->cbegin()); }
ChatServer::ConstClientsIterator ChatServer::Clients::cend()   const { return ConstClientsIterator(pClientsMap->cend()  ); }

size_t ChatServer::Clients::Size() const { return pClientsMap->size(); }
bool ChatServer::Clients::IsEmpty() const { return pClientsMap->empty(); };

ChatServer::ClientsIterator      ChatServer::Clients::Find(UserID_t id) { return ClientsIterator(pClientsMap->find(id)); }
ChatServer::ConstClientsIterator ChatServer::Clients::Find(UserID_t id) const { return ConstClientsIterator(pClientsMap->find(id)); }

bool ChatServer::Clients::Contains(UserID_t id) const { return (Find(id) !=  end()); }

}}  /*END OF NAMESPACES*/