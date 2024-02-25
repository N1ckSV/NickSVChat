
#ifndef _NICKSV_CHAT_CLIENT_T
#define _NICKSV_CHAT_CLIENT_T
#pragma once


#include <thread>
#include <exception>
#include <string>
#include <map>


#include "NickSV/Chat/Interfaces/IChatSocket.h"


namespace NickSV::Chat {

class ChatClient: public IChatSocket
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
    EResult Run(const SteamNetworkingIPAddr &serverAddr, ChatErrorMsg &errMsg  ) override;
    void CloseConnection() override;
    bool IsRunning();
    void SetInfo(ClientInfo* pClientInfo);
    ChatClient();
    ~ChatClient();

protected:
    virtual EResult OnPreStartConnection(const SteamNetworkingIPAddr &serverAddr, ChatErrorMsg &errMsg ) ;
    virtual EResult OnStartConnection(const SteamNetworkingIPAddr &serverAddr, ChatErrorMsg &errMsg )    ;
    virtual void OnPreCloseConnection()    ;
    virtual void OnCloseConnection()       ;

private:
    void ConnectionThreadFunction();
    void OnSteamNetConnectionStatusChanged( SteamNetConnectionStatusChangedCallback_t *pInfo );
    static void SteamNetConnectionStatusChangedCallback( SteamNetConnectionStatusChangedCallback_t *);
    std::thread* m_pConnectionThread;
    volatile bool m_bGoingExit;
    HSteamNetConnection m_hConnection;
    ISteamNetworkingSockets* m_pInterface;
    ClientInfo* m_pClientInfo;
    static ChatClient *s_pCallbackInstance;
};



} /*END OF NAMESPACES*/


#endif // _NICKSV_CHAT_CLIENT_T