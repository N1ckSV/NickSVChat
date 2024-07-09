
#ifndef _NICKSV_CHAT_IREQUEST_T
#define _NICKSV_CHAT_IREQUEST_T
#pragma once

#include "NickSV/Chat/Types.h"

namespace NickSV {
namespace Chat {



class IRequest
{
public:
    [[nodiscard]]  virtual ERequestType GetType() const = 0;
                   virtual              ~IRequest() = default;
};




}}  /*END OF NAMESPACES*/




#endif // _NICKSV_CHAT_IREQUEST_T