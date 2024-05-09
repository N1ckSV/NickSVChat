
#ifndef _NICKSV_CHAT_SERVER_T
#define _NICKSV_CHAT_SERVER_T
#pragma once


#include <utility>
#include <map>
#include <unordered_map>
#include <forward_list>

#include "NickSV/Chat/ChatSocket.h"

#include "NickSV/Tools/ValueLock.h"



namespace NickSV::Chat {



class ChatServer: public ChatSocket
{
public:
    ChatServer();
    virtual ~ChatServer();
    EResult Run(const ChatIPAddr &serverAddr = ChatIPAddr()) override final;
    //ClientsMap is client's connection to client info map
    using ClientsMap = std::map<HSteamNetConnection, std::unique_ptr<ClientInfo>>;
    //ConnectionsMap is client's id to client's connection map
    using ConnectionsMap = std::unordered_map<UserID_t, HSteamNetConnection>;

    // .first is a user id we failed send request to and
    // .second is an error result why we failed
    using IDResultList = std::forward_list<std::pair<UserID_t, EResult>>;

    // If we cant find client info with specific id,
    // ref to const InvalidClientInfo returned
    //
    // ATTENTION:
    // This function is NOT CLIENT STATE THREAD PROTECTED inside.
    // So in order to get and modify valid ClientInfo
    // lock UserID_t first by calling this->LockClient(UserID_t).
    ClientInfo&         GetClientInfo(UserID_t);

    // RETURNS:
    //  • Success if client is removed;
    //  • NoAction if client wasn't found
    EResult             RemoveClient(UserID_t);

    // Locks given UserID_t (Client) in current thread.
    // Don't forget to call UnlockClientInfo()
    // after handling given UserID_t.
    //
    // This works as a mutex:
    // 1) if UserID_t wasn't locked yet - locking it,
    //    so no other threads will* modify ClientInfo 
    //    binded to it until unlock called (* - its only
    //    guarunteed when other threads also locking it);
    // 2) if UserID_t already locked - blocking current thread
    //    and waiting for unlocking by other threads.
    //
    // THROWS:
    //     The same exceptions as ValueLock::lock(UserID_t) throws.
    //
    // ATTENTION: 
    //     When you overriding event functions that should work with
    //     UserID_t or ClientInfo binded to it
    //     and this function is called by ChatServer owned threads
    //     UserID_t could already be locked, so check description
    //     of overrided functions for CLIENT STATE THREAD PROTECTED 
    //     attention note
    void                LockClient(UserID_t);

    // Unlocks given UserID_t in current thread. See LockClientInfo().
    //
    // THROWS:
    //     The same exceptions as ValueLock::unlock(UserID_t) throws
    void                UnlockClient(UserID_t);

    // RETURNS:
    //  • EState::Busy     - UserID_t is busy
    //  • EState::Free     - UserID_t is free
    //  • EState::Reserved - UserID_t is reserved
    //
    // ATTENTION:
    // This function is NOT CLIENT STATE THREAD PROTECTED inside.
    // So if you did not call this->LockClient(UserID_t) (see decl.)
    // the real state of this UserID_t can be changed by other threads
    // just after this returned or during its call and
    // returned EState value can be different from real one.
    EState              GetUserIDState(UserID_t);
    EState              ReserveUserID(UserID_t);

    virtual EResult     OnPreAcceptClient(ConnectionInfo*);
    virtual void        OnAcceptClient(ConnectionInfo*, UserID_t, EResult);

    
     
    // rawRequest - a data we got from client
    //
    // userID - an ID of client we got request from
    //
    // result - read next
    //
    // This function is called when:
    // 1) incoming message cannot be treat as Request [result == InvalidRequest];
    // 2) client we got Request from is unauthorized and request 
    //    is not ClientInfoRequest [result == InvalidConnection];
    // 3) m_handleRequestsQueue is overflowed [result == Overflow].
    //
    // ATTENTION:
    // 1) This function is CLIENT STATE THREAD PROTECTED
    //    (called when ClientInfo with given userID is already locked,
    //    so no other threads should modify it and you do not want
    //    to lock this userID again to avoid DEADLOCK).
    // 2) All 3 function call cases (listed above)
    //    can happen at the same time but result will 
    //    still be InvalidRequest, because we are calling this
    //    function in the validating order given above, so when you got 
    //    InvalidRequest, check for an unauthorized client
    //    and m_handleRequestsQueue overflow by yourself. 
    //    FIXME m_handleRequestsQueue is private so api user cant check for overflow
    // 
    // NOTE:
    // OnBadIncomingRequest() does not supposed to be overrided
    // to handle API user added requests with other ERequestType. 
    // We don't treat API user defined requests as BAD. Because 
    // OnBadIncomingRequest() is called with result == InvalidRequest 
    // only if incoming data size is less than size of ERequestType,
    // so use OnHandleRequest() for this. This is also does not mean that
    // all requests that don't trigger OnBadIncomingRequest() are good.
    // They may be garbage and parsing them inside OnHandleRequest() 
    // will indicate their validity.
    virtual void    OnBadIncomingRequest(std::string rawRequest, UserID_t userID, EResult result);

    // Same as ChatSocket::OnErrorSendingRequest(), but when
    // server sending request to multiple clients and we failed
    // to send request to some of them with a specific result. See IDResultList.
    //
    // ATTENTION:
    // IDResultList is NOT CLIENT STATE THREAD PROTECTED
    // (so each ClientInfo binded to UserID_t in given failedList 
    // can be modified by other threads during this function call)
    virtual void    OnErrorSendingRequestToAll(std::string rawRequest, RequestInfo rInfoInitial, IDResultList failedList);
private:
    EResult         AcceptClient(ConnectionInfo*);
    
    // Returns Success if removed and NoAction if not found
    EResult         RemoveClient(HSteamNetConnection);
    UserID_t        GenerateUniqueUserID();
    void            ConnectionThreadFunction() override final;
	void            PollIncomingRequests()     override final;
	void            PollQueuedRequests()       override final;
	void            PollConnectionChanges()    override final;
    void            HandleClientInfoRequest(ClientInfoRequest*, RequestInfo) override final;
    void            HandleMessageRequest(MessageRequest*, RequestInfo)       override final;
    void            OnSteamNetConnectionStatusChanged(ConnectionInfo*)       override final;
    static void     SteamNetConnectionStatusChangedCallback(ConnectionInfo*);
    HSteamListenSocket       m_hListenSock;
	HSteamNetPollGroup       m_hPollGroup;
    //cppcheck-suppress unusedStructMember
    static ChatServer*       s_pCallbackInstance;
    //cppcheck-suppress unusedStructMember
	ClientsMap               m_mapClients;
    ConnectionsMap           m_mapConnections;
    std::mutex               m_handleClientInfoMutex;
    UserID_t                 m_lastFreeID = Constant::ApiReservedUserIDs;
    Tools::ValueLock<UserID_t, 2>   m_UserIDLock;
};



//struct ConnectionException : public std::exception {
//		const char* what() const throw () override {
//			return "Something went wrong with connection";
//		}
//	};
//




} /*END OF NAMESPACES*/


#endif // _NICKSV_CHAT_SERVER_T