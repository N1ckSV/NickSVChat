

#ifndef _NICKSV_CHAT_TYPES
#define _NICKSV_CHAT_TYPES
#pragma once


#include <cstdint>

#include "NickSV/Chat/Definitions.h"

#include "steam/steamnetworkingtypes.h"


namespace NickSV {
namespace Chat {



/** @property Client
 * 
 *  @brief Entity/term that means a client.
 * 
 *  The Client is not yet settled whether it means 
 *  the User or the unit of the running program 
 *  using the @ref ChatSocket .
 *  Also Client is not a C++ class (maybe not yet).
 *  The C++ class descriptor for Client is @ref ClientInfo .
 * 
 *  @todo define what Client means
*/


/** @property CLIENT_DEPENDENT_OBJECT
 * 
 *  @brief 
 *  A general object concept/property/term (not C++20 concept).
 * 
 *  This property tells that the object is somehow 
 *  binded to all @ref Client 's or a specific @ref Client
 *  managed by @ref ChatServer .
 *  For example:
 *  @ref ChatServer::m_hPollGroup ;
 *  @ref ChatServer::m_mapClients ;
 *  @ref ChatServer::m_mapConnections .
 *  So when you work with ChatServer's methods
 *  that are changing any info about @ref Client ,
 *  these methods are handling CLIENT_DEPENDENT_OBJECTs
 *  and should be marked with attention note if
 *  they are @ref CLIENT_STATE_THREAD_PROTECTED property or NOT.
 *  See @see CLIENT_STATE_THREAD_PROTECTED .
 * 
 *  @attention 
 *  This term is closely related to the CLIENT_STATE_THREAD_PROTECTED property.
 * 
*/


/** @property CLIENT_STATE_THREAD_PROTECTED
 * 
 *  @brief 
 *  A general object concept/property/term (not C++20 concept).
 * 
 *  This property tells that the function or method is
 *  handling @ref Client safely by locking special
 *  mutex-like object @ref ChatServer::m_ClientLock of
 *  class @ref ValueLock
 *  binded to all @ref Client 's or a specific @ref Client
 *  managed by @ref ChatServer .
 *  For example:
 *  @ref ChatServer::m_hPollGroup ;
 *  @ref ChatServer::m_mapClients ;
 *  @ref ChatServer::m_mapConnections .
 *  So when you work with ChatServer's methods
 *  that are changing any info about @ref Client ,
 *  these methods are handling CLIENT_DEPENDENT_OBJECTs
 *  and should be marked with attention note if
 *  they are @ref CLIENT_STATE_THREAD_PROTECTED property or NOT.
 *  See @see CLIENT_STATE_THREAD_PROTECTED .
 * 
 *  @attention 
 *  This term is closely related to the CLIENT_STATE_THREAD_PROTECTED property.
 * 
*/








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
    InvalidConnection,

    ClosedConnection
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
    constexpr static int MaxSendRequestsQueueSize = 100;
    //cppcheck-suppress unusedStructMember
    constexpr static int LibReservedUserIDs = 256;
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
    alignas(Type) char CharArr[sizeof(Type)]; 
};

}}  /*END OF NAMESPACES*/



#endif // _NICKSV_CHAT_TYPES