
#ifndef _NICKSV_CHAT_REQUEST_T
#define _NICKSV_CHAT_REQUEST_T
#pragma once

#include <future>

#include "NickSV/Chat/Interfaces/IRequest.h"
#include "NickSV/Chat/Interfaces/ISerializable.h"



namespace NickSV {
namespace Chat {


class NICKSVCHAT_API Request : public IRequest, public ISerializable
{
public:
    Request() = default;
    ERequestType GetType() const override;

    /**
     * @returns nullptr because Unknown Request shouldn't be serialized
     */
    const std::unique_ptr<ISerializer> GetSerializer() const override;
};

/**
 * @brief Struct to store sending info of Request
 */
struct NICKSVCHAT_API RequestInfo
{
    /**
     * @brief User ID to send Request to
     * or ignore depending on sendFlags
     * (SF_SEND_TO_ONE and SF_SEND_TO_ALL)
     */
    UserID_t userID;

    //All send flags starts with SF_
    uint64_t sendFlags;

    //Intended for LIB user purposes
    union ExtraInfo
    {
        uint64_t AsUINT;
        void*    AsPOINTER;
        char     AsARRAY[sizeof(uint64_t)];
    } extraInfo;
    
    RequestInfo();
    explicit RequestInfo(UserID_t id);
    RequestInfo(UserID_t id, uint64_t flags);
};



}}  /*END OF NAMESPACES*/




#endif // _NICKSV_CHAT_REQUEST_T