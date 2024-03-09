
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
private:
    EResult     SendStringToServer(std::string);
    void        ConnectionThreadFunction() override final;
    void        RequestThreadFunction()    override final;
    void        OnSteamNetConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t *pInfo) override final;
    static void SteamNetConnectionStatusChangedCallback(SteamNetConnectionStatusChangedCallback_t*);
    EResult     PollIncomingRequests()  override final;
	EResult     PollQueuedRequests()    override final;
	void        PollConnectionChanges() override final;
    HSteamNetConnection      m_hConnection;
    ClientInfo*              m_pClientInfo;
    static ChatClient*       s_pCallbackInstance;
};



} /*END OF NAMESPACES*/


#endif // _NICKSV_CHAT_CLIENT_T