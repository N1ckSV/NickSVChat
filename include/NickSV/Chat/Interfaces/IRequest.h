
#ifndef _NICKSV_CHAT_IREQUEST_T
#define _NICKSV_CHAT_IREQUEST_T
#pragma once

#include "NickSV/Chat/Types.h"

namespace NickSV::Chat {



class IRequest
{
public:
    CHAT_NODISCARD virtual ERequestType GetType() const = 0;
                   virtual              ~IRequest() = default;
};




} /*END OF NAMESPACES*/




#endif // _NICKSV_CHAT_IREQUEST_T