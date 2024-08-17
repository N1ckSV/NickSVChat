/**
 * @file
 * @brief All Library types are declared here
 * 
*/

#ifndef _NICKSV_CHAT_TYPES
#define _NICKSV_CHAT_TYPES
#pragma once


#include <cstdint>
#include <type_traits>

#include "steam/steamnetworkingtypes.h"

#include "NickSV/Chat/Definitions.h"


namespace NickSV {
namespace Chat {



/** 
 * @page Terms
 * @tableofcontents
 * 
 * @section Client
 * 
 * Entity/term that means a client.
 *
 * The Client is not yet settled whether it means 
 * the User or the unit of the running program 
 * using the ChatClient socket. 
 * Also Client is not a C++ class.
 * The C++ class descriptor for Client is ClientInfo.
 * ChatServer::HClient is class helper that holds 
 * ClientInfo and client's HSteamNetConnection
 * @todo define what Client means
 * @sa ClientInfo
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * @section CLIENT_DEPENDENT_OBJECT 
 *
 * A general object concept/property/term (not C++20 concept).
 *
 * This property tells that the object is somehow 
 * binded to all @ref Client "Clients" or specific @ref Client
 * managed by ChatServer.
 * 
 * So when you work with ChatServer's methods
 * that are changing any info about @ref Client ,
 * these methods are handling @ref CLIENT_DEPENDENT_OBJECT "CLIENT_DEPENDENT_OBJECTs"
 * and should be marked with attention note whether
 * they are @ref CLIENT_STATE_CONCURRENCY_PROTECTED or NOT
 * ( NOT @ref CLIENT_STATE_CONCURRENCY_PROTECTED ).
 * And if they are marked - you should unlock any specific @ref Client or 
 * @ref Client "Clients" if it is already done.
 * If they are not - you should lock specific @ref Client
 * or @ref Client "Clients" to avoid data races between threads.
 *
 * @subsection cd_obj_examples CLIENT_DEPENDENT_OBJECTs:
 * 
 * @ref ChatServer::m_hPollGroup, 
 * @ref ChatServer::m_mapClients, 
 * @ref ChatServer::m_mapConnections.
 *
 * @sa @ref CLIENT_STATE_CONCURRENCY_PROTECTED
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * @section CLIENT_STATE_CONCURRENCY_PROTECTED
 * 
 * A general object concept/property/term (not C++20 concept).
 *
 * This property tells that the function is
 * handling @ref CLIENT_DEPENDENT_OBJECT safely by locking special
 * mutex-like object @ref ChatServer::m_ClientLock of
 * class @ref DynamicValueLock (or not).
 * 
 * So when you work with ChatServer's methods
 * that are changing any info about @ref Client ,
 * these methods are handling @ref CLIENT_DEPENDENT_OBJECT "CLIENT_DEPENDENT_OBJECTs"
 * and should be marked with attention note whether
 * they are @ref CLIENT_STATE_CONCURRENCY_PROTECTED or NOT
 * ( NOT @ref CLIENT_STATE_CONCURRENCY_PROTECTED ).
 * And if they are marked - you should unlock any specific @ref Client or 
 * @ref Client "Clients" if it is already done.
 * If they are not - you should lock specific @ref Client
 * or @ref Client "Clients" to avoid data races between threads.
 * 
 * @attention
 * Additionally [NOT] @ref CLIENT_STATE_CONCURRENCY_PROTECTED can be marked with
 * word "inside" or "outside". "Inside" is for functions you're usually calling
 * by yourself and "outside" is for methods you are overriding.
 * 
 * @subsection cscp_example Example:
 * 
 * @code{.cpp}
 *  //Thread 1
 *  UserID_t id = ... ; // got id somehow
 *  GetClientLock().Lock(id); //locks id if Thread 2 not yet been locked all, blocks otherwise
 *  auto& client = GetClientInfo(id);
 *  if(&client != &InvalidClientInfo)
 *  {        
 *      //... do something with client
 *  }
 *  else
 *  {
 *      //... do something if client not found
 *  }
 *  GetClientLock().Unlock(id);
 *  //Thread 1 end
 * 
 *  //Thread 2
 *  UserID_t id = ... ; // got id somehow
 *  // locking all 
 *  // because removing client means 
 *  // changing CLIENT_DEPENDENT_OBJECT structure
 *  // and other clients can be moved
 *  GetClientLock().LockAll(); //locks all ids if Thread 1 not yet been locked its id, blocks otherwise
 *  auto result = RemoveClient(id);
 *  switch(result)
 *  {
 *  // cases and default
 *  }
 *  GetClientLock().UnlockAll();
 *  //Thread 2 end
 *  
 * @endcode
 * Code above can be also done with RAII objects: 
 * 1) @ref ChatServer::ClientLockGuard;
 * 2) @ref ChatServer::ClientLockAllGuard;
 * 3) @ref ChatServer::UniqueUnlocker;
 * or their combination.
 * 
 * @sa @ref CLIENT_DEPENDENT_OBJECT
*/




enum class EResult : uint32_t
{
    Success = 0,
    Error,

    AlreadyRunning,
    ClosedSocket,
    Overflow,
    NoAction,
    TimeoutExpired,
    
    InvalidSize,
    InvalidParam,
    InvalidRequest,
    InvalidConnection,

    ClosedConnection,

    Size         // Last value of EResult
};

enum class ERequestType : uint32_t
{
    Unknown = 0,
    Message,
    ClientInfo,

    Size         // Last value of ERequestType
};

enum class EState : uint32_t
{
    Invalid = 0, // Object is invalid

    Busy,        // ID is busy
    Free,        // ID is free
    Reserved,    // ID is reserved

    Active,      // Connected and ready to get messages
    Unauthorized,// Not ready to get messages

    Size         // Last value of EState
};


using Version_t = uint32_t;
using UserID_t = uint64_t;

struct Constant
{
    //cppcheck-suppress unusedStructMember
    constexpr static int MaxSendRequestsQueueSize = 100;
    //cppcheck-suppress unusedStructMember
    constexpr static int LibReservedUserIDs = 256;
    //cppcheck-suppress unusedStructMember
    constexpr static UserID_t ServerID = 1;
    //cppcheck-suppress unusedStructMember
    constexpr static UserID_t InvalidUserID = 0;
};

using ChatStatusMsg = char[1024];
using ChatErrorMsg = ChatStatusMsg;

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

template<class T>
class SafeQueue;

class TaskInfo;




}}  /*END OF NAMESPACES*/



#endif // _NICKSV_CHAT_TYPES