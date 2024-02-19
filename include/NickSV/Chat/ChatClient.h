

#ifndef _NICKSV_CHAT_CLIENT_T
#define _NICKSV_CHAT_CLIENT_T
#pragma once


#include <thread>
#include <exception>
#include <string>
#include <map>


#include "NickSV/Chat/IChatClient.h"


namespace NickSV::Chat {

class ChatClient: public IChatClient
{
public:
    void Run(const SteamNetworkingIPAddr &serverAddr ) override;
    void CloseConnection() override;
    bool IsRunning();
    void SetInfo(ClientInfo<>* pClientInfo);
    ChatClient();
    ~ChatClient();

protected:
    virtual void OnPreStartConnection() ; 
    virtual void OnStartConnection()    ; 
    virtual void OnPreCloseConnection() ; 
    virtual void OnCloseConnection()    ; 

private:
    void MainThreadFunction();
    void OnSteamNetConnectionStatusChanged( SteamNetConnectionStatusChangedCallback_t *pInfo );
    static void SteamNetConnectionStatusChangedCallback( SteamNetConnectionStatusChangedCallback_t *);
    std::thread* m_pMainThread;
    volatile bool m_bGoingExit;
    HSteamNetConnection m_hConnection;
    ISteamNetworkingSockets* m_pInterface;
    ClientInfo<>* m_pClientInfo;
    static ChatClient *s_pCallbackInstance;
};



} /*END OF NAMESPACES*/


#endif // _NICKSV_CHAT_CLIENT_T