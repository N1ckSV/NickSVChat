
#ifndef _NICKSV_CHAT_REQUEST_T
#define _NICKSV_CHAT_REQUEST_T
#pragma once


#include "NickSV/Chat/Interfaces/IRequest.h"



namespace NickSV::Chat {


class Request : public IRequest
{
public:
    EResult Handle() override;
    ERequestType GetType() const override;
};




} /*END OF NAMESPACES*/




#endif // _NICKSV_CHAT_REQUEST_T