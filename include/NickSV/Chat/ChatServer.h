
#ifndef _NICKSV_CHAT_SERVER_T
#define _NICKSV_CHAT_SERVER_T
#pragma once


#include <thread>
#include <exception>
#include <string>
#include <map>


#include "NickSV/Chat/Interfaces/IChatSocket.h"


namespace NickSV::Chat {

class ChatServer: public IChatSocket
{
public:
    EResult Run(const SteamNetworkingIPAddr &serverAddr, ChatErrorMsg &errMsg  ) override;
    void CloseConnection() override;
    bool IsRunning();
    ChatServer();
    ~ChatServer();

protected:
    virtual EResult OnPreStartConnection(const SteamNetworkingIPAddr &serverAddr, ChatErrorMsg &errMsg ) ;
    virtual EResult OnStartConnection(const SteamNetworkingIPAddr &serverAddr, ChatErrorMsg &errMsg )    ;
    virtual void OnPreCloseConnection()    ;
    virtual void OnCloseConnection()       ;

private:
    void ConnectionThreadFunction();
    void SendStringToClient( HSteamNetConnection, const char * );
    void SendStringToAllClients( const char *, HSteamNetConnection except = 0 );
    void OnSteamNetConnectionStatusChanged( SteamNetConnectionStatusChangedCallback_t * );
    static void SteamNetConnectionStatusChangedCallback( SteamNetConnectionStatusChangedCallback_t * );
    std::thread* m_pConnectionThread;
    volatile bool m_bGoingExit;
	HSteamListenSocket m_hListenSock;
	HSteamNetPollGroup m_hPollGroup;
	ISteamNetworkingSockets *m_pInterface;
    static ChatServer *s_pCallbackInstance;

	std::map< HSteamNetConnection, ClientInfo/*FIXME FIXME FIXME FIXME FIXME has to be pointer or think of it better*/> m_mapClients;
};



//struct ConnectionException : public std::exception {
//		const char* what() const throw () override {
//			return "Something went wrong with connection";
//		}
//	};
//




} /*END OF NAMESPACES*/


#endif // _NICKSV_CHAT_SERVER_T