
#ifndef _NICKSV_CHAT_MESSAGE_REQUEST_PASTE
#define _NICKSV_CHAT_MESSAGE_REQUEST_PASTE
#pragma once


#include "NickSV/Chat/BasicMessage.h"
#include "NickSV/Chat/Requests/MessageRequest.h"

namespace NickSV::Chat {


//-----------------------------------------------------------------------------------
// Impementation of MessageRequest
//-----------------------------------------------------------------------------------
EResult MessageRequest::Handle() { return EResult::Success; }

ERequestType MessageRequest::GetType() const { return ERequestType::Message; }
//-----------------------------------------------------------------------------------
// End
//-----------------------------------------------------------------------------------




} /*END OF NAMESPACES*/




#endif // _NICKSV_CHAT_MESSAGE_REQUEST_PASTE