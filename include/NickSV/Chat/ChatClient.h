
#ifndef _NICKSV_CHAT_CLIENT_T
#define _NICKSV_CHAT_CLIENT_T
#pragma once


#include "NickSV/Chat/ChatSocket.h"


namespace NickSV::Chat {

class ChatClient: public ChatSocket
{
    /*
    Description
    
    This is main client class handles connection to server, receiving messages,client information.
    This class is supposed to be inherited, but base work is also possible

    */
public:
    /*
    Initializes networking library and connection thread
    */
    ChatClient();
    virtual ~ChatClient();
    //IChatSocket impl
    //A.K.A. StartConnection
    EResult Run(const ChatIPAddr &serverAddr = ChatIPAddr()) override final;
    
    // rawRequest is a data we got from server
    //
    // result - read below
    //
    // This function is called when:
    // 1) Incoming message cannot be treat as Request [result == InvalidRequest];
    // 2) m_handleRequestsQueue is overflowed         [result == Overflow].
    //
    // ATTENTION: All 2 cases above can happen at the same time
    // but result will still be InvalidRequest, because we are calling this
    // function in the validating order given above, so when you got 
    // InvalidRequest, check for an unauthorized client (only in ChatServer)
    // and m_handleRequestsQueue overflow by yourself.
    // 
    // NOTE: OnBadIncomingRequest() does not supposed to be overrided
    // to handle API user added requests with other ERequestType. 
    // We don't treat API user defined requests as BAD. Because 
    // OnBadIncomingRequest() is called with result == InvalidRequest 
    // only if incoming data size is less than size of ERequestType,
    // so use OnPreHandleRequest() or OnHandleRequest() for this.
    // This is also does not mean that all requests that don't trigger
    // OnBadIncomingRequest() are good. They may be garbage and parsing 
    // them inside OnPreHandleRequest() or OnHandleRequest() 
    // will indicate their validity.
    virtual void    OnBadIncomingRequest(std::string rawRequest, EResult result);
private:
    EResult     SendStringToServer(const std::string*);
    void        ConnectionThreadFunction() override final;
    void        RequestThreadFunction()    override final;
    void        OnSteamNetConnectionStatusChanged(ConnectionInfo*) override final;
    static void SteamNetConnectionStatusChangedCallback(ConnectionInfo*);
    void        PollIncomingRequests()  override final;
	void        PollQueuedRequests()    override final;
	void        PollConnectionChanges() override final;
    HSteamNetConnection         m_hConnection;
    std::unique_ptr<ClientInfo> m_upClientInfo;
    static ChatClient*          s_pCallbackInstance;
};



} /*END OF NAMESPACES*/


#endif // _NICKSV_CHAT_CLIENT_T