
#ifndef _NICKSV_CHAT_CLIENT_T
#define _NICKSV_CHAT_CLIENT_T
#pragma once


#include "NickSV/Chat/ChatSocket.h"

#include <chrono>


namespace NickSV {
namespace Chat {

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
    
    /**
     * @brief A.K.A. StartConnection
     * 
     * @attention
     * For connect timeout and reconnect tries see @ref OnPreReconnect and @ref OnReconnect
     */
    EResult Run(const ChatIPAddr &serverAddr = ChatIPAddr()) override final;

    /**
     * @brief 
     * If the incoming request is bad 
     * according to lib or custom criteria
     * this function is called
     * 
     * @param rawRequest - a data we got from Server
     * @param result - read next
     *
     * @attention
     * This function is called when:
     * 1) Incoming message cannot be treat as Request [result == InvalidRequest];
     * 2) m_handleRequestsQueue is overflowed         [result == Overflow].
     *
     * @warning
     *     All 2 cases above can happen at the same time
     *     but result will still be InvalidRequest, because we are calling this
     *     function in the validating order given above.
     * 
     * @note
     *     OnBadIncomingRequest() does not supposed to be overrided
     *     to handle library user added requests with other ERequestType. 
     *     We don't treat library user defined requests as BAD. Because 
     *     OnBadIncomingRequest() is called with result == InvalidRequest 
     *     only if incoming data size is less than size of ERequestType,
     *     so use OnPreHandleRequest() or OnHandleRequest() for this.
     *     This is also does not mean that all requests that don't trigger
     *     OnBadIncomingRequest() are good. They may be garbage and parsing 
     *     them inside OnPreHandleRequest() or OnHandleRequest() 
     *     will indicate their validity.
    */
    virtual void OnBadIncomingRequest(std::string rawRequest, EResult result);
    ClientInfo&  GetClientInfo();
    
    using ChatSocket::HandleRequest;
    using ChatSocket::OnPreHandleRequest;
    using ChatSocket::OnHandleRequest;

    virtual EResult OnPreHandleRequest(ClientInfoRequest&, RequestInfo&);
    virtual EResult OnPreHandleRequest(MessageRequest&, RequestInfo&);
    
    virtual void    OnHandleRequest(const ClientInfoRequest&, RequestInfo, EResult, TaskInfo);
    virtual void    OnHandleRequest(const MessageRequest&, RequestInfo, EResult);

    /**
     * @param serverAddr  by default, this is one you set in ChatClient::Run()
     * @param timeout     for how long Connect() should try to connect (500 ms minimum)
     * @param maxAttempts how many times Connect() should try to connect (1 time minimum),
     * tries evenly distributed over the timeout and timeout should be 
     * greater or equal than 500ms*maxAttempts (at least 500ms per attempt)
     * 
     * @attention Timeout of Connect() starts to work just after this method is called
     * 
     * @warning   timeout should be greater or equal than maxAttempts (at least 1ms per attempt)
     */
    virtual EResult OnPreConnect(ChatIPAddr &serverAddr, std::chrono::milliseconds &timeout, unsigned int &maxAttempts);
    /**
     * @attention Timeout of Connect() ends before this method is called
     */
    virtual void    OnConnect(const ChatIPAddr &serverAddr, EResult res, std::chrono::milliseconds reconnectTime, unsigned int countRetries);
    /**
     * @attention Timeout of Connect() includes this method's lifetime
     */
    virtual void    OnConnectAttemptFailed(ChatIPAddr &serverAddr, std::chrono::milliseconds reconnectTime, unsigned int countRetries, unsigned int maxAttempts);

private:
    EResult      Connect(ChatIPAddr &serverAddr, std::chrono::milliseconds timeout = std::chrono::milliseconds(10000), unsigned int maxAttempts = 10u);
    EResult      SendStringToServer(const std::string&);
    static void  SteamNetConnectionStatusChangedCallback(ConnectionInfo*);
    void         ConnectionThreadFunction() override final;
    void         PollIncomingRequests()     override final;
	void         PollQueuedRequests()       override final;
	void         PollConnectionChanges()    override final;
    void         OnSteamNetConnectionStatusChanged(ConnectionInfo*) override final;
    EResult      HandleRequest(ClientInfoRequest&, RequestInfo)     override final;
    EResult      HandleRequest(MessageRequest&   , RequestInfo)     override final;
    ChatIPAddr                  m_serverIPAddress;
    HSteamNetConnection         m_hConnection;
    std::unique_ptr<ClientInfo> m_upClientInfo;
    static ChatClient*          s_pCallbackClientInstance;
};



}}  /*END OF NAMESPACES*/


#endif // _NICKSV_CHAT_CLIENT_T