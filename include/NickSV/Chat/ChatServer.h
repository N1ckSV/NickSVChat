
#ifndef _NICKSV_CHAT_SERVER_T
#define _NICKSV_CHAT_SERVER_T
#pragma once


#include <utility>
#include <map>
#include <unordered_map>
#include <forward_list>

#include "NickSV/Chat/ChatSocket.h"



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
    ClientInfo&         GetClientInfo(UserID_t);

    EState              GetUserIDState(UserID_t);
    EState              ReserveUserID(UserID_t);

    virtual EResult     OnPreAcceptClient(ConnectionInfo*);
    virtual void        OnAcceptClient(ConnectionInfo*, UserID_t, EResult);
     
    // rawRequest is a data we got from client
    //
    // userID is an ID of client we got request from
    //
    // result - read below
    //
    // This function is called when:
    // 1) incoming message cannot be treat as Request [result == InvalidRequest];
    // 2) client we got Request from is unauthorized and request 
    //    is not ClientInfoRequest [result == InvalidConnection];
    // 3) m_handleRequestsQueue is overflowed [result == Overflow].
    //
    // ATTENTION: All 3 cases above can happen at the same time
    // but result will still be InvalidRequest, because we are calling this
    // function in the validating order given above, so when you got 
    // InvalidRequest, check for an unauthorized client
    // and m_handleRequestsQueue overflow by yourself.
    // 
    // NOTE: OnBadIncomingRequest() does not supposed to be overrided
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
    // to send request to some of them with a specific result. See IDResultList
    virtual void        OnErrorSendingRequestToAll(std::string rawRequest, RequestInfo rInfoInitial, IDResultList failedList);
private:
    EResult     AcceptClient(ConnectionInfo*);
    // FIXME TODO mb need to add public RemoveClient(UserID_t) and OnRemoveClient etc.
    // Returns Success if removed and NoAction if not found
    EResult     RemoveClient(HSteamNetConnection);
    void        ConnectionThreadFunction() override final;
    void        RequestThreadFunction()    override final;
    UserID_t    GenerateUniqueUserID();
    void        OnSteamNetConnectionStatusChanged(ConnectionInfo*) override final;
    static void SteamNetConnectionStatusChangedCallback(ConnectionInfo*);
	void        PollIncomingRequests()  override final;
	void        PollQueuedRequests()    override final;
	void        PollConnectionChanges() override final;
    HSteamListenSocket       m_hListenSock;
	HSteamNetPollGroup       m_hPollGroup;
    //cppcheck-suppress unusedStructMember
    static ChatServer*       s_pCallbackInstance;
	ClientsMap               m_mapClients;
    ConnectionsMap           m_mapConnections;
    UserID_t                 m_lastFreeID = Constant::ApiReservedUserIDs;
};



//struct ConnectionException : public std::exception {
//		const char* what() const throw () override {
//			return "Something went wrong with connection";
//		}
//	};
//




} /*END OF NAMESPACES*/


#endif // _NICKSV_CHAT_SERVER_T