
#ifndef _NICKSV_CHAT_SOCKET_T
#define _NICKSV_CHAT_SOCKET_T
#pragma once


#include <thread>
#include <exception>
#include <string>
#include <map>
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
    /*
    Initializes networking library and connection thread
    */
    ChatSocket();
    virtual ~ChatSocket();

    //IChatSocket impl
    //A.K.A. StartConnection
    EResult Run(const ChatIPAddr &serverAddr = ChatIPAddr()) override;
    void    CloseConnection()                                override final;
    bool    IsRunning()                                      override final;
    EResult QueueRequest(const Request*, RequestInfo)        override final;
    EResult HandleRequest(const Request*, RequestInfo)       override final;

    using RequestsQueue = std::queue<std::pair<std::string, RequestInfo>>;

    virtual EResult OnPreRun(const ChatIPAddr &serverAddr, ChatErrorMsg &errMsg);
    virtual EResult OnRun(const ChatIPAddr &serverAddr, EResult outsideResult, ChatErrorMsg &errMsg);
    virtual void    OnPreCloseConnection();
    virtual void    OnCloseConnection();
    virtual EResult OnPreHandleRequest(const Request*, RequestInfo&);
    virtual EResult OnHandleRequest(const Request*, RequestInfo, EResult outsideResult);
    virtual EResult OnPreQueueRequest(const Request*, RequestInfo&);
    virtual EResult OnQueueRequest(const Request*, RequestInfo, EResult outsideResult);

protected:
    virtual void    ConnectionThreadFunction();
    virtual void    RequestThreadFunction()    = 0;
    virtual void    OnSteamNetConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t *pInfo) = 0;
    virtual EResult PollIncomingRequests()  = 0;
	virtual EResult PollQueuedRequests()    = 0;
	virtual void    PollConnectionChanges() = 0;
    std::thread*             m_pConnectionThread;
    std::thread*             m_pRequestThread;
    volatile bool            m_bGoingExit;
    ISteamNetworkingSockets* m_pInterface;
    RequestsQueue            m_sendRequestsQueue;
    RequestsQueue            m_handleRequestsQueue;
    std::mutex               m_sendRequestMutex;
    std::mutex               m_handleRequestMutex;
};



} /*END OF NAMESPACES*/


#endif // _NICKSV_CHAT_SOCKET_T