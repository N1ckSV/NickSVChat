
#ifndef _NICKSV_CHAT_SOCKET_T
#define _NICKSV_CHAT_SOCKET_T
#pragma once


#include <thread>
#include <exception>
#include <string>
#include <queue>
#include <memory>
#include <mutex>
#include <tuple>


#include <steam/isteamnetworkingsockets.h>


#include "NickSV/Chat/ClientInfo.h"
#include "NickSV/Chat/Interfaces/IChatSocket.h"
#include "NickSV/Chat/Requests/Request.h"
#include "NickSV/Chat/Utils.h"

#include "NickSV/Tools/Memory.h"


namespace NickSV {
namespace Chat {




class NICKSVCHAT_API ChatSocket: public IChatSocket
{
    /*
    Description:
    This abstract class is declaration/implementation of 
    common functions between ChatClient and ChatServer.
    ChatClient and ChatServer inherited from this class.

    */
public:
    ChatSocket();
    virtual ~ChatSocket();

    //A.K.A. StartSocket, StartConnection, StartClient
    EResult Run(const ChatIPAddr &serverAddr = ChatIPAddr()) override;
    void    CloseSocket()                                    override final;
    bool    IsRunning()                                      override final;
    // Same as IsRunning() but blocks current thread  
    // and waits for all socket threads to finish 
    void    Wait()                                           override final;

    /**
     * @brief Send Request to other socket.
     * Just calls return QueueRequest(...).GetInitialResult()
     * 
     * @param rReq ref to Request, should be known request object
     * (e.g ClientInfoRequest, MessageRequest or LIB user defined)
     * @param reqInfo request sending information
     * (ID of user to send to etc.)
     * 
     * @return EResult of QUEUEING that request, NOT SENDING.
     * For EResult of SENDING see @ref QueueRequest().
     * 
     * @sa @ref RequestInfo
     * @sa @ref QueueRequest
     * 
     * @todo better comment
     * 
     * @attention
     * RequestInfo ignored in ChatClient socket.
     * OnPreQueueRequest and OnQueueRequest also triggers here
     */
    EResult SendRequest(  Request& rReq, RequestInfo reqInfo = RequestInfo{ Constant::InvalidUserID }) override final;

    /**
     * @brief Queue Request for sending to other socket.
     * Same as SendRequest, but with information 
     * whether the request was sent
     * 
     * @param rReq ref to Request, should be known request object
     * (e.g ClientInfoRequest or MessageRequest)
     * @param reqInfo request sending information
     * (ID of user to send to etc.)
     * 
     * @return TaskInfo object that holds EResult of QUEUEING that request, NOT SENDING,
     * and std::future<EResult> with EResult of SENDING.
     * 
     * @sa @ref RequestInfo
     * 
     * @todo better comment
     * 
     * @attention
     * RequestInfo ignored in ChatClient socket
     */
    TaskInfo QueueRequest(Request& rReq, RequestInfo reqInfo = RequestInfo{ Constant::InvalidUserID, SF_SEND_TO_ONE }) override final;

    EResult  HandleRequest(Request& rReq, RequestInfo reqInfo) override final;

    using SendRequestsQueue_t = SafeQueue<std::tuple<std::string, RequestInfo, std::promise<EResult>>>;
    using HandleRequestsQueue_t = SafeQueue<std::tuple<std::string, RequestInfo>>;

    virtual EResult OnPreRun(const ChatIPAddr &serverAddr, ChatErrorMsg&);
    virtual EResult OnPreQueueRequest( Request&, RequestInfo&);
    virtual EResult OnPreHandleRequest(Request&, RequestInfo&);
    virtual void    OnPreCloseSocket();

    virtual void    OnRun(const ChatIPAddr &serverAddr, EResult outsideResult, ChatErrorMsg&);
    virtual void    OnCloseSocket();
    virtual void    OnQueueRequest( const Request&, RequestInfo, EResult queueResult);
    virtual void    OnHandleRequest(const Request&, RequestInfo, EResult result);
    /**
     * @brief Function name says everything
     * 
     * @param pInfo ConnectionInfo (alias of GNS type SteamNetConnectionStatusChangedCallback_t)
     * 
     * @attention
     * Library user needs to include GNS headers to work 
	 * with overrided OnConnectionStatusChanged
     */
    virtual void    OnConnectionStatusChanged(ConnectionInfo *pInfo, EResult);


    /**
     * @brief Function name says everything
     * 
     * @param pInfo ConnectionInfo (alias of GNS type SteamNetConnectionStatusChangedCallback_t)
     * 
     * @attention
     * Library user needs to include GNS headers to work 
	 * with overrided OnConnectionStatusChanged
     */
    virtual void    OnErrorSendingRequest(std::string rawRequest, RequestInfo, EResult);
    virtual void    OnFatalError(const std::string& errorMsg);
    
    /**
     * @brief Call it when fatal error occured
     * 
     * @warning Automatically calls CloseSocket()
     * 
     * @param errorMsg error message
     */
    void            FatalError(const std::string& errorMsg = "Unknown Error");

    virtual std::unique_ptr<ClientInfo>  MakeClientInfo();

protected:
    virtual void    ConnectionThreadFunction();
    virtual void    RequestThreadFunction();
    virtual void    PollIncomingRequests()  = 0;
	virtual void    PollQueuedRequests()    = 0;
	virtual void    PollConnectionChanges() = 0;
    virtual void    OnSteamNetConnectionStatusChanged(ConnectionInfo *pInfo) = 0;
    virtual EResult HandleRequest(ClientInfoRequest&, RequestInfo);
    virtual EResult HandleRequest(MessageRequest&, RequestInfo);
            EResult SendStringToConnection(HSteamNetConnection, const std::string&);
    
    //cppcheck-suppress unusedStructMember
    std::thread*             m_pConnectionThread;
    //cppcheck-suppress unusedStructMember
    std::thread*             m_pRequestThread;
    //cppcheck-suppress unusedStructMember
    volatile bool            m_bGoingExit;
    //cppcheck-suppress unusedStructMember
    ISteamNetworkingSockets* m_pInterface;
    //cppcheck-suppress unusedStructMember
    SendRequestsQueue_t      m_sendRequestsQueue;
    HandleRequestsQueue_t    m_handleRequestsQueue;
};



}}  /*END OF NAMESPACES*/


#endif // _NICKSV_CHAT_SOCKET_T