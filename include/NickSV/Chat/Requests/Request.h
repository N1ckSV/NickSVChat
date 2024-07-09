
#ifndef _NICKSV_CHAT_REQUEST_T
#define _NICKSV_CHAT_REQUEST_T
#pragma once

#include <future>

#include "NickSV/Chat/Interfaces/IRequest.h"
#include "NickSV/Chat/Interfaces/ISerializable.h"



namespace NickSV {
namespace Chat {


class Request : public IRequest, public ISerializable
{
public:
    Request() = default;
    ERequestType GetType() const override;
    const std::unique_ptr<ISerializer> GetSerializer() const override;
};

/**
 * @brief Struct to store some info
 *        about Request
 * 
 * @warning Move-only
 */
struct RequestInfo
{
    UserID_t id;
    //All send flags starts with SF_
    uint32_t sendFlags;
    RequestInfo();
    //cppcheck-suppress noExplicitConstructor
    RequestInfo(UserID_t id);
    RequestInfo(UserID_t id, uint32_t flags);
};




}}  /*END OF NAMESPACES*/




#endif // _NICKSV_CHAT_REQUEST_T