

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
    Success = 0,
    Error,
    AlreadyRunning,
    InvalidSize
};

enum class ERequestType : uint32_t
{
    Unknown = 0,
    Message,
    ClientInfo
};

struct Constant
{
    static const int MaxChatErrorMsgSize  = 1024;
    static const int MaxNicknameSize  = MAX_NICKNAME_SIZE;
};

using ChatErrorMsg = char[Constant::MaxChatErrorMsgSize];

using Version_t = uint32_t;

class IChatSocket;
class ChatClient;
class ChatServer;


/*
"Basic" prefix here means the same thing as "basic_" in std::basic_string<...>
*/

template<typename CharT = CHAT_CHAR>
class BasicClientInfo;
using ClientInfo = BasicClientInfo<CHAT_CHAR>;


class ISerializer;
class ISerializable;
template<typename Serializable>
class Serializer;
template<typename Serializable>
class ConstSerializer;

using ClientInfoSerializer = Serializer<ClientInfo>;
using ConstClientInfoSerializer = ConstSerializer<ClientInfo>;


template<typename CharT = CHAT_CHAR>
class BasicMessage;
using Message = BasicMessage<CHAT_CHAR>;

using MessageSerializer = Serializer<Message>;
using ConstMessageSerializer = ConstSerializer<Message>;


class SendInfo;


class IRequest;
class Request;
class MessageRequest;
class ClientInfoRequest;


using RequestSerializer = Serializer<Request>;
using MessageRequestSerializer = Serializer<MessageRequest>;
using ClientInfoRequestSerializer = Serializer<ClientInfoRequest>;


} /*END OF NAMESPACES*/



#endif // _NICKSV_CHAT_TYPES