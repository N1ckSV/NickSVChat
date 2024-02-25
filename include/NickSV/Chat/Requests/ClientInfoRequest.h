
#ifndef _NICKSV_CHAT_CLIENTINFO_REQUEST_T
#define _NICKSV_CHAT_CLIENTINFO_REQUEST_T
#pragma once


#include "NickSV/Chat/Requests/Request.h"



namespace NickSV::Chat {


class ClientInfoRequest : public Request
{
public:
    EResult Handle() override;
    ERequestType GetType() const override;
};




} /*END OF NAMESPACES*/




#endif // _NICKSV_CHAT_CLIENTINFO_REQUEST_T