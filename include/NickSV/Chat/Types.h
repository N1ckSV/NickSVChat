

#ifndef _NICKSV_CHAT_TYPES
#define _NICKSV_CHAT_TYPES
#pragma once


#include <cstdint>

#include "NickSV/Chat/Defines.h"


//GameNetworkingSockets types/
//This is added so the API user doesn't need to install GameNetworkingSockets' headers
class SteamNetworkingIPAddr;
class SteamNetConnectionStatusChangedCallback_t;
class ISteamNetworkingSockets;
/// Handle used to identify a connection to a remote host.
typedef uint32_t HSteamNetConnection;
/// Handle used to identify a "listen socket".  Unlike traditional
/// Berkeley sockets, a listen socket and a connection are two
/// different abstractions.
typedef uint32_t HSteamListenSocket;
/// Handle used to identify a poll group, used to query many
/// connections at once efficiently.
typedef uint32_t HSteamNetPollGroup;




namespace NickSV::Chat {

enum EResult
{
    kSucces = 0,
    kError
};

using Version_t = uint32_t;

class IChatClient;
class ChatClient;
class IChatServer;
class ChatServer;

template<class CharT = TCHAR>
class ClientInfo;

template<typename Serializable>
class Serializer;

template<typename CharT = TCHAR>
using ClientInfoSerializer = Serializer<ClientInfo<CharT>>;



} /*END OF NAMESPACES*/



#endif // _NICKSV_CHAT_TYPES