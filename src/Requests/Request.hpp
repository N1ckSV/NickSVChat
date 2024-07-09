
#ifndef _NICKSV_CHAT_REQUEST_PASTE
#define _NICKSV_CHAT_REQUEST_PASTE
#pragma once



#include "NickSV/Chat/Requests/Request.h"
#include "NickSV/Chat/Interfaces/ISerializer.h"

namespace NickSV {
namespace Chat {


//-----------------------------------------------------------------------------------
// Impementation of Unknown (default) Request
//-----------------------------------------------------------------------------------
ERequestType Request::GetType() const { return ERequestType::Unknown; }

const std::unique_ptr<ISerializer> Request::GetSerializer() const
{
    return std::unique_ptr<ISerializer>(nullptr);
}
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------


//-----------------------------------------------------------------------------------
// Impementation of RequestInfo
//-----------------------------------------------------------------------------------
RequestInfo::RequestInfo() : id(0), sendFlags(0) {};
RequestInfo::RequestInfo(UserID_t _id) : id(_id), sendFlags(0) {};
RequestInfo::RequestInfo(UserID_t _id, uint32_t flags) : id(_id), sendFlags(flags) {};
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------




}}  /*END OF NAMESPACES*/




#endif // _NICKSV_CHAT_REQUEST_PASTE