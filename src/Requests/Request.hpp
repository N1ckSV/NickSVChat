
#ifndef _NICKSV_CHAT_REQUEST_PASTE
#define _NICKSV_CHAT_REQUEST_PASTE
#pragma once



#include "NickSV/Chat/Requests/Request.h"

namespace NickSV::Chat {


//-----------------------------------------------------------------------------------
// Impementation of Unknown (default) Request
//-----------------------------------------------------------------------------------
EResult Request::Handle() { return EResult::Success; }

ERequestType Request::GetType() const { return ERequestType::Unknown; }
//-----------------------------------------------------------------------------------
// End
//-----------------------------------------------------------------------------------





} /*END OF NAMESPACES*/




#endif // _NICKSV_CHAT_REQUEST_PASTE