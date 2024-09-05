
#ifndef _NICKSV_CHAT_REQUEST_PASTE
#define _NICKSV_CHAT_REQUEST_PASTE
#pragma once



#include "NickSV/Chat/Interfaces/ISerializer.h"
#include "NickSV/Chat/Parsers/RequestParser.h"
#include "NickSV/Chat/Requests/Request.h"

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
RequestInfo::RequestInfo() : userID(0), sendFlags(0), extraInfo{0} {};
RequestInfo::RequestInfo(UserID_t _id) : userID(_id), sendFlags(0), extraInfo{0} {};
RequestInfo::RequestInfo(UserID_t _id, uint64_t flags) : userID(_id), sendFlags(flags), extraInfo{0} {};
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------




}}  /*END OF NAMESPACES*/




#endif // _NICKSV_CHAT_REQUEST_PASTE