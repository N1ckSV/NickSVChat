

#ifndef _NICKSV_CHAT_TYPES
#define _NICKSV_CHAT_TYPES
#pragma once


#include <cstdint>

#include "NickSV/Chat/Defines.h"

#include "steam/steamnetworkingtypes.h"


namespace NickSV::Chat {

enum class EResult
{
    Success = 0,
    Error,

    AlreadyRunning,
    Overflow,
    NoAction,
    
    InvalidSize,
    InvalidParam,
    InvalidRequest,
    InvalidConnection
};

enum class ERequestType : uint32_t
{
    Unknown = 0,
    Message,
    ClientInfo
};

enum class EState : uint32_t
{
    Invalid = 0, // Object is invalid

    Busy,        // ID is busy
    Free,        // ID is free
    Reserved,    // ID is reserved

    Active,      // Connected and ready to get messages
    Unauthorized // Not ready to get messages
};

struct Constant
{
    //cppcheck-suppress unusedStructMember
    static const int MaxSendRequestsQueueSize = 100;
    //cppcheck-suppress unusedStructMember
    static const int ApiReservedUserIDs = 256;
};

using ChatStatusMsg = char[1024];
using ChatErrorMsg = ChatStatusMsg;

using Version_t = uint32_t;
using UserID_t = uint64_t;

using ConnectionInfo = SteamNetConnectionStatusChangedCallback_t;

class IChatSocket;
class ChatClient;
class ChatServer;

struct ChatIPAddr;

class ClientInfo;

class ISerializer;
class IParser;
class ISerializable;

template<typename Serializable>
class SimplySerializable;

template<typename Serializable>
class Serializer;

template<typename Parsable>
class Parser;

using ClientInfoSerializer = Serializer<ClientInfo>;
using ClientInfoParser = Parser<ClientInfo>;


/*
"Basic" prefix here means the same thing as "basic_" in std::basic_string<...>
*/
template<typename CharT = CHAT_CHAR>
class BasicMessage;
using Message = BasicMessage<CHAT_CHAR>;


class RequestInfo;


class IRequest;
class Request;
class MessageRequest;
class ClientInfoRequest;


using RequestSerializer = Serializer<Request>;
using RequestParser = Parser<Request>;

using MessageRequestSerializer = Serializer<MessageRequest>;
using MessageRequestParser = Parser<MessageRequest>;

template<typename Type>
union Transfer { 
    Type Base; 
    char CharArr[sizeof(Type)]; 
};


} /*END OF NAMESPACES*/



#endif // _NICKSV_CHAT_TYPES