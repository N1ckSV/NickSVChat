
#ifndef _NICKSV_CHAT_IPARSER_T
#define _NICKSV_CHAT_IPARSER_T
#pragma once

#include "NickSV/Chat/Types.h"




namespace NickSV {
namespace Chat {



//------------------------------------------------------------------------------------
// Parser interface
//------------------------------------------------------------------------------------
class NICKSVCHAT_API IParser
{
public:
    [[nodiscard]]  virtual std::string::const_iterator        FromString(const std::string&) = 0;
    [[nodiscard]]  virtual std::string::const_iterator        FromString(std::string::const_iterator, std::string::const_iterator) = 0;
    [[nodiscard]]  virtual std::string::const_iterator        OnFromString(std::string::const_iterator, std::string::const_iterator) = 0;
                   virtual                                    ~IParser() = default;
};
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------








}}  /*END OF NAMESPACES*/




#endif // _NICKSV_CHAT_IPARSER_T