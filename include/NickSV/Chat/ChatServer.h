
#ifndef _NICKSV_CHAT_SERVER_T
#define _NICKSV_CHAT_SERVER_T
#pragma once


#include <utility>
#include <map>
#include <unordered_map>
#include <forward_list>

#include "NickSV/Tools/ValueLock.h"

#include "NickSV/Chat/ChatSocket.h"
#include "NickSV/Chat/ClientInfo.h"




namespace NickSV {
namespace Chat {




class NICKSVCHAT_API ChatServer: public ChatSocket
{
public:
    ChatServer();
    virtual ~ChatServer();
    EResult Run(const ChatIPAddr &serverAddr = ChatIPAddr()) override final;

    struct NICKSVCHAT_API HClient
    {
        HSteamNetConnection Connection;
        ClientInfo clientInfo;
    };
    /**
     * @brief std::map UserID -> pair of connection and ClientInfo
     * 
     * @sa @ref ConnectionsMap_t
     */
    using ClientsMap_t = std::map<UserID_t, HClient>;
    /**
     * @brief std::unordered_map connection -> UserID
     * 
     * @sa @ref ClientsMap_t
     */
    using ConnectionsMap_t = std::unordered_map<HSteamNetConnection, UserID_t>;

    // .first is a user id we failed send request to and
    // .second is an error result why we failed
    using IDResultList_t = std::forward_list<std::pair<UserID_t, EResult>>;

    /** 
     * @brief
     * ClientLock_t ( @ref NickSV::Tools::DynamicValueLock<UserID_t> ) is a class
     * that helps to control @ref CLIENT_DEPENDENT_OBJECT through all threads
     * by locking and unlocking internal std::mutex'es.
     * See description of @ref NickSV::Tools::DynamicValueLock for more info.
     *
     * 
     * @details
     * @ref CLIENT_DEPENDENT_OBJECT :
     *     @ref m_mapClients, @ref m_mapConnections, @ref m_hPollGroup and etc.
     *
     * ClientLock_t methods:
     *     Lock(UserID_t) - locks @ref Client with given UserID_t;
     *     Unlock(UserID_t) - unlocks @ref Client with given UserID_t;
     *     LockAll() - locks all @ref Client "Clients" ;
     *     UnlockAll() - unlocks all @ref Client "Clients" ;
     *     LockAll(UserID_t) - locks all @ref Client "Clients" except given UserID_t;
     *     UnlockAll(UserID_t) - unlocks all @ref Client "Clients" except given UserID_t;
     *     You can combine these methods.
     * 
     * @sa @ref GetClientLock()
     * 
     * @note
     * Locking two different Clients is allowed
     *
     * @warning
     * 1) Remember - you don't want to lock the same @ref Client twice,
     * because ClientLock_t will lock inner std::mutex twice and deadlock possible.
     * See description of @ref NickSV::Tools::ValueLock for more info.
     * 
     * @warning
     * 2) When you overriding event methods that 
     * should work with @ref CLIENT_DEPENDENT_OBJECT
     * and you need to lock specific @ref Client or lock all @ref Client "Clients",
     * this may have already been done by an external method,
     * so check description of overrided methods for 
     * @ref CLIENT_STATE_CONCURRENCY_PROTECTED attention note
     * 
     */
    using ClientLock_t = Tools::DynamicValueLock<UserID_t>;
    using UniqueUnlocker = std::unique_ptr<ClientLock_t, ClientLock_t::Unlocker>;
    using ClientLockGuard = Tools::ValueLockGuard<ClientLock_t>;
    using ClientLockAllGuard = Tools::ValueLockAllGuard<ClientLock_t>;

    class NICKSVCHAT_API ClientsIterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = ClientInfo;
        using difference_type = std::ptrdiff_t;
        using pointer = ClientInfo*;
        using reference = ClientInfo&;

        explicit ClientsIterator(ClientsMap_t::iterator it);

        reference operator*() const;
        pointer operator->() const;

        ClientsIterator& operator++();
        ClientsIterator operator++(int);

        friend bool NICKSVCHAT_API operator==(const ClientsIterator& a, const ClientsIterator& b);
        friend bool NICKSVCHAT_API operator!=(const ClientsIterator& a, const ClientsIterator& b);

    private:
        ClientsMap_t::iterator  it_;
    };

    class NICKSVCHAT_API ConstClientsIterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = ClientInfo;
        using difference_type = std::ptrdiff_t;
        using pointer = const ClientInfo*;
        using reference = const ClientInfo&;

        explicit ConstClientsIterator(ClientsMap_t::const_iterator it);

        reference operator*() const;
        pointer operator->() const;

        ConstClientsIterator& operator++();
        ConstClientsIterator operator++(int);

        friend bool operator==(const ConstClientsIterator& a, const ConstClientsIterator& b);
        friend bool operator!=(const ConstClientsIterator& a, const ConstClientsIterator& b);

    private:
        ClientsMap_t::const_iterator  it_;
    };


    /**
     * @brief Helper class to manage @ref ClientInfo "ClientInfos" easily.
     * 
     * @details
     * Works like map of UserID_t as key and @ref ClientInfo as value.
     * Not insertable, you only allowed to edit already 
     * containing @ref ClientInfo "ClientInfos".
     * All getters throw exceptions if key is not in Clients.
     * 
     * @note
     * To iterate over all @ref ClientInfo "ClientInfos" 
     * use GetClients().begin() and .end()
     */
    class NICKSVCHAT_API Clients
    {
    public:
        Clients() = delete;
        explicit Clients(ClientsMap_t* const _pClientsMap);
        DECLARE_RULE_OF_5_DELETE(Clients);
        
        /**
         * @throws - std::out_of_range if the Clients does not have an client with the specified UserID_t.
         */
        inline ClientInfo& Get(UserID_t id);
        /**
         * @throws - std::out_of_range if the Clients does not have an client with the specified UserID_t.
         */
        inline const ClientInfo& Get(UserID_t id) const;

        /**
         * @throws - std::out_of_range if the Clients does not have an client with the specified UserID_t.
         */
        inline ClientInfo& At(UserID_t id);
        /**
         * @throws - std::out_of_range if the Clients does not have an client with the specified UserID_t.
         */
        const ClientInfo& At(UserID_t id) const;

        /**
         * @throws - std::out_of_range if the Clients does not have an client with the specified UserID_t.
         */
        ClientInfo& operator[](UserID_t id);
        /**
         * @throws - std::out_of_range if the Clients does not have an client with the specified UserID_t.
         */
        const ClientInfo& operator[](UserID_t id) const;

        ClientsIterator begin();
        ClientsIterator end();

        ConstClientsIterator begin() const;
        ConstClientsIterator end()   const;

        ConstClientsIterator cbegin() const;
        ConstClientsIterator cend()   const;

        size_t Size() const;
        bool IsEmpty() const;

        ClientsIterator      Find(UserID_t id);
        ConstClientsIterator Find(UserID_t id) const;

        bool Contains(UserID_t id) const;

    private:
        ClientsMap_t* const pClientsMap;
    };

    Clients GetClients();

    /**
     * @brief ClientInfo getter by UserID
     * 
     * @returns - Valid client info if it is exist for given UserID
     * @returns - @ref InvalidClientInfo if no such client info
     *
     * @attention
     * This function is NOT @ref CLIENT_STATE_CONCURRENCY_PROTECTED inside
     * 
    */          
    ClientInfo&         GetClientInfo(UserID_t);

    /** 
     * @brief Removes client with given ID, also
     * closing connection
     * 
     * @attention
     * This function is @ref CLIENT_STATE_CONCURRENCY_PROTECTED inside
     * 
     * @retval - Success          - if Client found and removed;
     * @retval - ClosedConnection - if Client is not found, but the connection is found and released;
     * @retval - NoAction         - if Client and connection are not found
     * @retval - InvalidParam     - if given ID is less than Constant::LibReservedUserIDs
    */
    EResult             RemoveClient(UserID_t);


    /** 
     * @brief
     * Returns ref to ChatServer's ClientLock_t
     * 
     * @sa ClientLock_t
     */
    ClientLock_t&       GetClientLock();

    /**
     * @brief
     * Changing UserID.
     * 
     * @attention
     * This function is NOT @ref CLIENT_STATE_CONCURRENCY_PROTECTED inside
     * 
     * @warning
     * 1) Dangerous method if client with currentID is in Active EState,
     * so in this case - better to change their state to Unauthorized,
     * then wait some time for server to hanle all request to that client
     * 
     * @warning
     * 2) LIB protocol requires of client to know their UserID.
     * So after changing it send ClientInfoRequest to them with
     * new ID.
     */
    EResult             ChangeClientID(UserID_t currentID, UserID_t newID);

    /**
     * 
     * @retval - EState::Busy - UserID_t is busy     
     *           (if ID is reserved by Lib or m_mapClients contains ID with valid HSteamNetConnection)
     * @retval - EState::Reserved - UserID_t is reserved 
     *           (m_mapClients contains ID with k_HSteamNetConnection_Invalid)
     * @retval - EState::Free - UserID_t is free     
     *           (m_mapClients not contains ID)
     * 
     * @attention
     * This function is NOT @ref CLIENT_STATE_CONCURRENCY_PROTECTED inside
    */
    EState              GetUserIDState(UserID_t);
    

    /** 
     * @brief Reserves @ref UserID_t.
     * 
     *  Reserves only if it was in EState::Free state
     * 
     * @returns old state of given UserID
     * 
     * @param id UserID_t to reserve
     * 
     * @retval - EState::Free     - UserID_t has been successully reserved
     *                              and you are allowed to use it;
     * @retval - EState::Reserved - UserID_t is already reserved
     *                              and you are NOT allowed to use it;
     * @retval - EState::Busy     - UserID_t is already reserved and binded to Client
     *                              and you are NOT allowed to use it;
     * 
     * @attention
     * This function is NOT @ref CLIENT_STATE_CONCURRENCY_PROTECTED inside
     * 
    */ 
    EState              ReserveUserID(UserID_t id);

    /**
     * @param rReqInfo request info that will be set to send authorization request to accepted client,
     * rReqInfo's UserID is a unique one generated before the call, 
     * it will be set as UserID of accepted user if it will return Success
     * 
     * @attention
     * This function is NOT @ref CLIENT_STATE_CONCURRENCY_PROTECTED outside,
     * because ClientInfo and UserID_t have not yet been created.
    */
    virtual EResult     OnPreAcceptClient(const ConnectionInfo& conInfo, RequestInfo& rReqInfo);

    /**
     * @param rReqInfo request info that will be set to send authorization request to accepted client
     * 
     * @param rClientInfo ClientInfo of a client we accepting,
     * if accepting is failed rClientInfo can be InvalidClientInfo
     * 
     * @attention
     * This function is @ref CLIENT_STATE_CONCURRENCY_PROTECTED inside
     * 
     * @warning Do not wait for TaskInfo's future result inside,
     * because the send task is handled by the same thread after call of OnAcceptClient
    */
    virtual void        OnAcceptClient(const ConnectionInfo& conInfo, const RequestInfo& rReqInfo, ClientInfo& rClientInfo, EResult acceptRes, TaskInfo taskInfo);

    
     
    /**
     * @brief 
     * If the incoming request is bad 
     * according to lib or custom criteria
     * this function is called
     * 
     * @param rawRequest - a data we got from Client
     * @param rClientInfo - a ref to Client's ClientInfo we got request from
     * @param result - read next
     *
     * @attention
     * This function is called when:
     * 1) incoming message cannot be treat as Request [result == InvalidRequest];
     * 2) client we got Request from is unauthorized and request 
     *    is not ClientInfoRequest [result == InvalidConnection];
     * 3) Handling queue (m_handleRequestsQueue) is overflowed [result == Overflow].
     *
     * @attention
     * This function is @ref CLIENT_STATE_CONCURRENCY_PROTECTED outside by given client
     * 
     * @warning
     *  2) All 3 function call cases (listed above)
     *     can happen at the same time but result will 
     *     still be InvalidRequest, because we are calling this
     *     function in the validating order given above
     * 
     * @note
     *     OnBadIncomingRequest() does not supposed to be overrided
     *     to handle library user added requests with other ERequestType. 
     *     We don't treat library user defined requests as BAD. Because 
     *     OnBadIncomingRequest() is called with result == InvalidRequest 
     *     only if incoming data size is less than size of ERequestType,
     *     so use OnHandleRequest() for this. This is also does not mean that
     *     all requests that don't trigger OnBadIncomingRequest() are good.
     *     They may be garbage and parsing them inside OnHandleRequest()
     *     will indicate their validity.
    */
    virtual void    OnBadIncomingRequest(std::string rawRequest, ClientInfo& rClientInfo, EResult result);

    /** 
     * @brief
     * Same as ChatSocket::OnErrorSendingRequest(), but when
     * server sending request to multiple clients and we failed
     * to send request to some of them with a specific result.
     * 
     * @sa IDResultList_t.
     *
     * @attention
     * This function is NOT @ref CLIENT_STATE_CONCURRENCY_PROTECTED outside
     */
    virtual void    OnErrorSendingRequestToAll(std::string rawRequest, RequestInfo rInfoInitial, IDResultList_t failedList);

    using ChatSocket::HandleRequest;
    using ChatSocket::OnPreHandleRequest;
    using ChatSocket::OnHandleRequest;

    /**
     * @brief Event method that called before ChatServer
     * handling ClientInfoRequest
     * 
     * @todo important info should be placed here
     */
    virtual EResult OnPreHandleRequest(ClientInfoRequest&, RequestInfo&);
    virtual EResult OnPreHandleRequest(MessageRequest&, RequestInfo&);

    /**
     * @brief Event method that called after ChatServer
     * handled ClientInfoRequest
     * 
     * @details
     * If HandleRequest method is made the given @ref Client authorized
     * (their state is EState::Active and handleRes == EResult::Success) 
     * - it is sending authorization confirmation request.
     * If HandleRequest processed ClientInfoRequest as invalid 
     * (handleRes == EResult::InvalidRequest) 
     * - it is sending new request with reauthorization.
     * In both cases sendTaskInfo tells the results of these sending tasks,
     * otherwise ignore sendTaskInfo.
     * 
     * @param req handled request,
     * can be changed by OnPreHandleRequest or HandleRequest
     * @param reqInfo info about request,
     * can be changed by OnPreHandleRequest
     * @param clientInfo ref to info about Client we got request from,
     * can be InvalidClientInfo (handleRes should be one of errors)
     * @param handleRes EResult of HandleRequest proccessing
     * @param sendTaskInfo info with results of sending new request task, 
     * only useful in cases mentioned above. 
     * 
     * @attention
     * This function is @ref CLIENT_STATE_CONCURRENCY_PROTECTED outside
     * 
     * @warning Unlike OnAcceptClient you can wait for TaskInfo's future result inside,
     * because the send task is handled by another thread
     * 
     * @todo important info should be placed here
     */
    virtual void    OnHandleRequest(const ClientInfoRequest& req, RequestInfo reqInfo, ClientInfo& clientInfo, EResult handleRes, TaskInfo sendTaskInfo);

    /**
     * 
     * @warning Unlike OnAcceptClient you can wait for TaskInfo's future result inside,
     * because the send task is handled by another thread
     */
    virtual void    OnHandleRequest(const MessageRequest&, RequestInfo,  EResult handleRes, TaskInfo);

private:
    EResult         AcceptClient(ConnectionInfo&);
    

    ClientInfo&     GetClientInfoByConnection(HSteamNetConnection);
    /**
     * @brief Removes client with given HSteamNetConnection, also
     * closing connection
     * 
     * @attention
     * This function is @ref CLIENT_STATE_CONCURRENCY_PROTECTED inside
     *
     * @retval - Success          - if Client found and removed;
     * @retval - ClosedConnection - if Client is not found, but the connection is found and released;
     * @retval - NoAction         - if Client and connection are not found
     * @retval - InvalidParam     - if given HSteamNetConnection is k_HSteamNetConnection_Invalid
    */
    EResult         RemoveClientByConnection(HSteamNetConnection);

    /**
     * @todo see method body
     */
    UserID_t        GenerateUniqueUserID();
    static void     SteamNetConnectionStatusChangedCallback(ConnectionInfo*);
    void            ConnectionThreadFunction() override final;
	void            PollIncomingRequests()     override final;
	void            PollQueuedRequests()       override final;
	void            PollConnectionChanges()    override final;
    void            OnSteamNetConnectionStatusChanged(ConnectionInfo*) override final;
    EResult         HandleRequest(ClientInfoRequest&, RequestInfo)     override final;
    EResult         HandleRequest(MessageRequest&, RequestInfo)        override final;
    HSteamListenSocket       m_hListenSock;
	HSteamNetPollGroup       m_hPollGroup;
    //cppcheck-suppress unusedStructMember
    static ChatServer*       s_pCallbackServerInstance;
    //cppcheck-suppress unusedStructMember
	ClientsMap_t             m_mapClients;
    //cppcheck-suppress unusedStructMember
    ConnectionsMap_t         m_mapConnections;
    UserID_t                 m_lastFreeID = Constant::LibReservedUserIDs;
    ClientLock_t             m_ClientLock;
};





}}  /*END OF NAMESPACES*/


#endif // _NICKSV_CHAT_SERVER_T