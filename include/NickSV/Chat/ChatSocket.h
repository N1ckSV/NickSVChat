
#ifndef _NICKSV_CHAT_SOCKET_T
#define _NICKSV_CHAT_SOCKET_T
#pragma once


#include <thread>
#include <exception>
#include <string>
#include <queue>
#include <memory>
#include <mutex>


#include "NickSV/Chat/Interfaces/IChatSocket.h"

#include "NickSV/Chat/Utils.h"


namespace NickSV::Chat {

class ChatSocket: public IChatSocket
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
    EResult QueueRequest( Request*, RequestInfo)             override final;
    EResult HandleRequest(Request*, RequestInfo)             override final;

    using RequestsQueue = std::queue<std::pair<std::string, RequestInfo>>;

    virtual EResult OnPreRun(const ChatIPAddr &serverAddr, ChatErrorMsg&);
    virtual EResult OnPreHandleRequest(Request*, RequestInfo&);
    virtual EResult OnPreQueueRequest( Request*, RequestInfo&);
    virtual void    OnPreCloseSocket();
    virtual void    OnRun(const ChatIPAddr &serverAddr, EResult outsideResult, ChatErrorMsg&);
    virtual void    OnCloseSocket();
    virtual void    OnHandleRequest(const Request*, RequestInfo, EResult outsideResult);
    virtual void    OnQueueRequest( const Request*, RequestInfo, EResult outsideResult);
	// Parameter ConnectionInfo* pInfo is alias of GNS type
    // SteamNetConnectionStatusChangedCallback_t
	// and api user needs to include GNS headers to work 
	// with overrided OnConnectionStatusChanged
    virtual void    OnConnectionStatusChanged(ConnectionInfo *pInfo, EResult);
    virtual void    OnErrorSendingRequest(std::string, RequestInfo, EResult);
    virtual void    OnFatalError(const std::string& errorMsg);
    // Fatal error means socket going to shut down. FIXME add some error codes
            void    FatalError(const std::string& errorMsg = "Unknown Error");

    virtual std::unique_ptr<ClientInfo>  MakeClientInfo();

protected:
    virtual void    ConnectionThreadFunction();
    virtual void    RequestThreadFunction() = 0;
    virtual void    PollIncomingRequests()  = 0;
	virtual void    PollQueuedRequests()    = 0;
	virtual void    PollConnectionChanges() = 0;
    virtual void    OnSteamNetConnectionStatusChanged(ConnectionInfo *pInfo) = 0;
         EResult    SendStringToConnection(HSteamNetConnection, const std::string*);
    //cppcheck-suppress unusedStructMember
    std::thread*             m_pConnectionThread;
    //cppcheck-suppress unusedStructMember
    std::thread*             m_pRequestThread;
    //cppcheck-suppress unusedStructMember
    volatile bool            m_bGoingExit;
    //cppcheck-suppress unusedStructMember
    ISteamNetworkingSockets* m_pInterface;
    //cppcheck-suppress unusedStructMember
    RequestsQueue            m_sendRequestsQueue;
    RequestsQueue            m_handleRequestsQueue;
    std::mutex               m_sendRequestMutex;
    std::mutex               m_handleRequestMutex;
};



} /*END OF NAMESPACES*/


#endif // _NICKSV_CHAT_SOCKET_T