
#ifndef _NICKSV_CHAT_CLIENTINFO_REQUEST_PASTE
#define _NICKSV_CHAT_CLIENTINFO_REQUEST_PASTE
#pragma once


#include "NickSV/Chat/BasicClientInfo.h"
#include "NickSV/Chat/Requests/ClientInfoRequest.h"

namespace NickSV::Chat {


//-----------------------------------------------------------------------------------
// Impementation of ClientInfoRequest
//-----------------------------------------------------------------------------------
EResult ClientInfoRequest::Handle() { return EResult::Success; }

ERequestType ClientInfoRequest::GetType() const { return ERequestType::ClientInfo; }
//-----------------------------------------------------------------------------------
// End
//-----------------------------------------------------------------------------------




} /*END OF NAMESPACES*/




#endif // _NICKSV_CHAT_BCLIENTINFO_REQUEST_PASTE