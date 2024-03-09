
#ifndef _NICKSV_CHAT_SERVER_T
#define _NICKSV_CHAT_SERVER_T
#pragma once


#include <utility>

#include "NickSV/Chat/ChatSocket.h"



namespace NickSV::Chat {



class ChatServer: public ChatSocket
{
public:
    ChatServer();
    virtual ~ChatServer();
    EResult Run(const ChatIPAddr &serverAddr = ChatIPAddr()) override final;
    using ClientsMap = std::map<HSteamNetConnection, std::shared_ptr<ClientInfo>>;
private:
    void        ConnectionThreadFunction() override final;
    void        RequestThreadFunction()    override final;
    EResult     SendStringToClient(HSteamNetConnection, const std::string);
    size_t      SendStringToAllClients(const std::string, HSteamNetConnection except = 0);
    UserID_t    GenerateUniqueUserID();
    void        OnSteamNetConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t*) override final;
    static void SteamNetConnectionStatusChangedCallback(SteamNetConnectionStatusChangedCallback_t*);
	EResult     PollIncomingRequests()  override final;
	EResult     PollQueuedRequests()    override final;
	void        PollConnectionChanges() override final;
    HSteamListenSocket       m_hListenSock;
	HSteamNetPollGroup       m_hPollGroup;
    static ChatServer*       s_pCallbackInstance;
	ClientsMap               m_mapClients;
};



//struct ConnectionException : public std::exception {
//		const char* what() const throw () override {
//			return "Something went wrong with connection";
//		}
//	};
//




} /*END OF NAMESPACES*/


#endif // _NICKSV_CHAT_SERVER_T