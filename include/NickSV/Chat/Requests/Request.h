
#ifndef _NICKSV_CHAT_REQUEST_T
#define _NICKSV_CHAT_REQUEST_T
#pragma once


#include "NickSV/Chat/Interfaces/IRequest.h"
#include "NickSV/Chat/Interfaces/ISerializable.h"



namespace NickSV::Chat {


class Request : public IRequest, public ISerializable
{
public:
    Request() = default;
    ERequestType GetType() const override;
    const std::unique_ptr<ISerializer> GetSerializer() const override;
};

struct RequestInfo
{
    HSteamNetConnection connection;
    //All send flags starts with SF_
    uint32_t sendFlags;
    RequestInfo();
    RequestInfo(HSteamNetConnection conn);
    RequestInfo(HSteamNetConnection conn, uint32_t flags);
};




} /*END OF NAMESPACES*/




#endif // _NICKSV_CHAT_REQUEST_T