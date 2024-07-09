
#ifndef _NICKSV_CHAT_SERVER_T
#define _NICKSV_CHAT_SERVER_T
#pragma once


#include <utility>
#include <map>
#include <unordered_map>
#include <forward_list>

#include "NickSV/Chat/ChatSocket.h"

#include "NickSV/Tools/ValueLock.h"



namespace NickSV {
namespace Chat {




class ChatServer: public ChatSocket
{
public:
    ChatServer();
    virtual ~ChatServer();
    EResult Run(const ChatIPAddr &serverAddr = ChatIPAddr()) override final;
    //ClientsMap_t is client's connection to client info map
    using ClientsMap_t = std::map<HSteamNetConnection, std::unique_ptr<ClientInfo>>;
    //ConnectionsMap_t is client's id to client's connection map
    using ConnectionsMap_t = std::unordered_map<UserID_t, HSteamNetConnection>;

    // .first is a user id we failed send request to and
    // .second is an error result why we failed
    using IDResultList_t = std::forward_list<std::pair<UserID_t, EResult>>;

    // ClientLock_t (same as Tools::ValueLock<UserID_t, 2>) is a class
    // that helps to control CLIENT_DEPENDENT_OBJECT through all threads
    // by locking and unlocking internal std::mutex'es.
    // See description of NickSV::Tools::ValueLock for more info.
    //
    // See also: m_ClientLock and GetClientLock()
    //
    // CLIENT_DEPENDENT_OBJECT:
    //     m_mapClients, m_mapConnections, m_hPollGroup and etc.
    //
    // ClientLock_t methods:
    //     Lock(UserID_t) - locks Client with given UserID_t;
    //     Unlock(UserID_t) - unlocks Client with given UserID_t;
    //     LockAll() - locks all Clients;
    //     UnlockAll() - unlocks all Clients;
    //     LockAll(UserID_t) - locks all Clients except given UserID_t;
    //     UnlockAll(UserID_t) - unlocks all Clients except given UserID_t;
    //     You can combine these methods.
    //
    // ATTENTION:
    //     1) Remember - you don't want to lock the same Client twice,
    //        because ClientLock_t will lock inner std::mutex twice and deadlock possible
    //        (See description of NickSV::Tools::ValueLock for more info).
    // 
    //     2) When you overriding event methods that 
    //        should work with CLIENT_DEPENDENT_OBJECT
    //        and you need to lock all Clients or specific Client,
    //        this may have already been done by an external method,
    //        so check description of overrided methods for 
    //        CLIENT_STATE_THREAD_PROTECTED attention note
    //
    using ClientLock_t = Tools::ValueLock<UserID_t, 2>;
    using UniqueUnlocker = std::unique_ptr<ClientLock_t, ClientLock_t::Unlocker>;
    using ClientLockGuard = Tools::ValueLockGuard<ClientLock_t>;
    using ClientAllLockGuard = Tools::ValueLockAllGuard<ClientLock_t>;

    // If we cant find client info with specific id
    // reference to const InvalidClientInfo returned
    //
    // ATTENTION:
    //     This function is NOT CLIENT_STATE_THREAD_PROTECTED inside.
    //     So in order to get and modify valid ClientInfo
    //     your code should look like this:
    //
    //          UserID_t id = ... ; // got id somehow
    //          GetClientLock().LockAll();
    //          auto& client = GetClientInfo(id);
    //          if(&client != &InvalidClientInfo)
    //          {        
    //              GetClientLock().UnlockAll(id);
    //              ... do something with client
    //              GetClientLock().Unlock(id);
    //          }
    //          else
    //          {
    //              GetClientLock().UnlockAll();
    //              ... do something if client not found
    //          }
    //
    //     Code above can be also done with 
    //     ClientLockGuard or ClientLockAllGuard or UniqueUnlocker
    //     or their combination.
    //          
    ClientInfo&         GetClientInfo(UserID_t);

    /** 
     * @todo not implemented yet
    */
    EResult             RemoveClient(UserID_t);


    /** 
     * @brief Returns reference to ClientLock_t
     * 
     * ClientLock_t (same as Tools::ValueLock<UserID_t, 2>) object
     * helps to control @ref CLIENT_DEPENDENT_OBJECT through all threads
     * by locking and unlocking internal std::mutex'es.
     * See description of NickSV::Tools::ValueLock for more info.
     *
     *
     * ClientLock_t methods:
     *     Lock(UserID_t) - locks Client with given UserID_t;
     *     Unlock(UserID_t) - unlocks Client with given UserID_t;
     *     LockAll() - locks all Clients;
     *     UnlockAll() - unlocks all Clients;
     *     LockAll(UserID_t) - locks all Clients except given UserID_t;
     *     UnlockAll(UserID_t) - unlocks all Clients except given UserID_t;
     *     You can combine these methods.
     *
     * @attention
     *     1) Remember - you don't want to lock the same Client twice,
     *        because ClientLock_t will lock inner std::mutex twice and deadlock possible
     *        (See description of NickSV::Tools::ValueLock for more info).
     * 
     *     2) When you overriding event methods that 
     *        should work with CLIENT_DEPENDENT_OBJECT
     *        and you need to lock all Clients or specific Client,
     *        this may have already been done by an external method,
     *        so check description of overrided methods for 
     *        CLIENT_STATE_THREAD_PROTECTED attention note
    */
    ClientLock_t&       GetClientLock();

    // RETURNS:
    //  • EState::Busy     - UserID_t is busy
    //  • EState::Free     - UserID_t is free
    //  • EState::Reserved - UserID_t is reserved
    //
    // ATTENTION:
    //     This function is NOT CLIENT_STATE_THREAD_PROTECTED inside.
    //     So if you did not call this->GetClientLock().Lock(UserID_t),
    //     the real state of this UserID_t can be changed by other threads
    //     just after this returned or during its call and
    //     returned EState value can be different from real one.
    EState              GetUserIDState(UserID_t);

    /** 
     * @brief Reserves @ref UserID_t.
     * 
     *  Reserves only if it was in EState::Free state.
     *  Returns old UserID_t state.
     * 
     * @param id UserID_t to reserve
     * 
     * @return - EState::Free     - UserID_t has been successully reserved
     *                              and you are allowed to use it;
     * @return - EState::Reserved - UserID_t is already reserved
     *                              and you are NOT allowed to use it;
     * @return - EState::Busy     - UserID_t is already reserved and binded to Client
     *                              and you are NOT allowed to use it;
     * 
     * @attention
     *     This function is CLIENT_STATE_THREAD_PROTECTED inside
     * @warning
     *     CLIENT_STATE_THREAD_PROTECTED inside means that you should
     *     not lock UserID_t before calling this function
     *     or there will be a DEADLOCK
    */ 
    EState              ReserveUserID(UserID_t id);

    /**
     * @attention
     *     This function is NOT CLIENT_STATE_THREAD_PROTECTED outside
     *     because ClientInfo and UserID_t have not yet been created.
    */
    virtual EResult     OnPreAcceptClient(ConnectionInfo& rInfo);

    /**
     * @attention
     *     This function is CLIENT_STATE_THREAD_PROTECTED outside.
    */
    virtual void        OnAcceptClient(ConnectionInfo& rInfo, ClientInfo* pClientInfo, EResult result);

    
     
    // rawRequest - a data we got from Client
    //
    // rClientInfo - a ref to Client's ClientInfo we got request from
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
    //  1) This function is CLIENT_STATE_THREAD_PROTECTED outside
    //     (called when Client with given pClientInfo is already locked,
    //     so no other threads should modify it and you do not want
    //     to lock this Client again to avoid DEADLOCK).
    //  2) All 3 function call cases (listed above)
    //     can happen at the same time but result will 
    //     still be InvalidRequest, because we are calling this
    //     function in the validating order given above, so when you got 
    //     InvalidRequest, check for an unauthorized client
    //     and m_handleRequestsQueue overflow by yourself. 
    //     FIXME m_handleRequestsQueue is private so library user cant check for overflow
    // 
    // NOTE:
    //     OnBadIncomingRequest() does not supposed to be overrided
    //     to handle library user added requests with other ERequestType. 
    //     We don't treat library user defined requests as BAD. Because 
    //     OnBadIncomingRequest() is called with result == InvalidRequest 
    //     only if incoming data size is less than size of ERequestType,
    //     so use OnHandleRequest() for this. This is also does not mean that
    //     all requests that don't trigger OnBadIncomingRequest() are good.
    //     They may be garbage and parsing them inside OnHandleRequest() 
    //     will indicate their validity.
    virtual void    OnBadIncomingRequest(std::string rawRequest, ClientInfo& rClientInfo, EResult result);

    // Same as ChatSocket::OnErrorSendingRequest(), but when
    // server sending request to multiple clients and we failed
    // to send request to some of them with a specific result. See IDResultList_t.
    //
    // ATTENTION:
    //     IDResultList_t is NOT CLIENT_STATE_THREAD_PROTECTED
    //     (so each Client binded to UserID_t in given failedList 
    //     can be modified by other threads during this function call)
    virtual void    OnErrorSendingRequestToAll(std::string rawRequest, RequestInfo rInfoInitial, IDResultList_t failedList);
private:
    EResult         AcceptClient(ConnectionInfo&);
    
    /**
     * @return - Success          - if Client found and removed;
     * @return - ClosedConnection - if Client is not found, 
     *                              but the connection is found and released;
     * @return - NoAction         - if Client and connection are not found
    */
    EResult         RemoveClient(HSteamNetConnection);
    UserID_t        GenerateUniqueUserID();
    static void     SteamNetConnectionStatusChangedCallback(ConnectionInfo*);
    void            ConnectionThreadFunction() override final;
	void            PollIncomingRequests()     override final;
	void            PollQueuedRequests()       override final;
	void            PollConnectionChanges()    override final;
    void            OnSteamNetConnectionStatusChanged(ConnectionInfo*)       override final;
    EResult         HandleClientInfoRequest(ClientInfoRequest&, RequestInfo) override final;
    EResult         HandleMessageRequest(MessageRequest&, RequestInfo)       override final;
    HSteamListenSocket       m_hListenSock;
	HSteamNetPollGroup       m_hPollGroup;
    //cppcheck-suppress unusedStructMember
    static ChatServer*       s_pCallbackServerInstance;
    //cppcheck-suppress unusedStructMember
	ClientsMap_t             m_mapClients;
    ConnectionsMap_t         m_mapConnections;
    UserID_t                 m_lastFreeID = Constant::LibReservedUserIDs;
    ClientLock_t             m_ClientLock;
};



//struct ConnectionException : public std::exception {
//		const char* what() const throw () override {
//			return "Something went wrong with connection";
//		}
//	};
//




}}  /*END OF NAMESPACES*/


#endif // _NICKSV_CHAT_SERVER_T