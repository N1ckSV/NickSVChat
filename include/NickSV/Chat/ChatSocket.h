
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

#include "NickSV/Tools/Memory.h"


namespace NickSV::Chat {




template<typename T>
using NotNull = Tools::NotNull<T>;




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
    void    HandleRequest(Request*, RequestInfo)             override final;

    using RequestsQueue_t = std::queue<std::pair<std::string, RequestInfo>>;

    virtual EResult OnPreRun(const ChatIPAddr &serverAddr, ChatErrorMsg&);
    virtual EResult OnPreQueueRequest( Request*, RequestInfo&);
    virtual EResult OnPreHandleRequest(Request*, RequestInfo&);
    virtual EResult OnPreHandleClientInfoRequest(ClientInfoRequest*, RequestInfo);
    virtual EResult OnPreHandleMessageRequest(MessageRequest*, RequestInfo);
    virtual void    OnPreCloseSocket();

    virtual void    OnRun(const ChatIPAddr &serverAddr, EResult outsideResult, ChatErrorMsg&);
    virtual void    OnCloseSocket();
    virtual void    OnQueueRequest( const Request*, RequestInfo, EResult outsideResult);
    virtual void    OnHandleRequest(const Request*, RequestInfo, EResult outsideResult);
    virtual void    OnHandleClientInfoRequest(const ClientInfoRequest*, RequestInfo, EResult outsideResult);
    virtual void    OnHandleMessageRequest(const MessageRequest*, RequestInfo, EResult outsideResult);
	// Parameter ConnectionInfo* pInfo is alias of GNS type
    // SteamNetConnectionStatusChangedCallback_t
	// and library user needs to include GNS headers to work 
	// with overrided OnConnectionStatusChanged
    virtual void    OnConnectionStatusChanged(ConnectionInfo *pInfo, EResult);

    // Function name says everything.
    //
    // ATTENTION for ChatServer socket:
    //     This function is CLIENT STATE THREAD PROTECTED
    //     (id field of given RequestInfo is locked, 
    //     so no other threads should modify ClientInfo binded to it
    //     and you do not want to lock this RequestInfo's id again to avoid DEADLOCK).
    virtual void    OnErrorSendingRequest(std::string msg, RequestInfo, EResult);
    virtual void    OnFatalError(const std::string& errorMsg);
    // Fatal error means socket going to shut down. FIXME add some error codes
            void    FatalError(const std::string& errorMsg = "Unknown Error");

            bool    ValidateClientInfo(const ClientInfo* pClientInfo);
    virtual bool    OnValidateClientInfo(const ClientInfo* pClientInfo);

    virtual std::unique_ptr<ClientInfo>  MakeClientInfo();

protected:
    virtual void    ConnectionThreadFunction();
    virtual void    RequestThreadFunction();
    virtual void    PollIncomingRequests()  = 0;
	virtual void    PollQueuedRequests()    = 0;
	virtual void    PollConnectionChanges() = 0;
    virtual void    OnSteamNetConnectionStatusChanged(ConnectionInfo *pInfo) = 0;
         EResult    SendStringToConnection(HSteamNetConnection, const std::string*);
    
    virtual void    HandleClientInfoRequest(ClientInfoRequest*, RequestInfo);
    virtual void    HandleMessageRequest(MessageRequest*, RequestInfo);
    //cppcheck-suppress unusedStructMember
    std::thread*             m_pConnectionThread;
    //cppcheck-suppress unusedStructMember
    std::thread*             m_pRequestThread;
    //cppcheck-suppress unusedStructMember
    volatile bool            m_bGoingExit;
    //cppcheck-suppress unusedStructMember
    ISteamNetworkingSockets* m_pInterface;
    //cppcheck-suppress unusedStructMember
    RequestsQueue_t          m_sendRequestsQueue;
    RequestsQueue_t          m_handleRequestsQueue;
    struct 
    {
        std::mutex           sendRequestMutex;
        std::mutex           handleRequestMutex;
    } m_Mutexes;
};



} /*END OF NAMESPACES*/


#endif // _NICKSV_CHAT_SOCKET_T