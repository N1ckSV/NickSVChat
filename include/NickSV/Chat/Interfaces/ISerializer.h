
#ifndef _NICKSV_CHAT_ISERIALIZER_T
#define _NICKSV_CHAT_ISERIALIZER_T
#pragma once


#include <string>

#include "NickSV/Chat/Types.h"




namespace NickSV {
namespace Chat {




//------------------------------------------------------------------------------------
// Serializer interface
//------------------------------------------------------------------------------------
class NICKSVCHAT_API ISerializer
{
public:
    [[nodiscard]]  virtual inline               size_t GetSize() const = 0;
    [[nodiscard]]  virtual inline               size_t OnGetSize(size_t baseSize = 0) const = 0;

    [[nodiscard]]  virtual                 std::string ToString() const = 0;
    [[nodiscard]]  virtual       std::string::iterator ToString(std::string::iterator, std::string::iterator) const = 0;
    [[nodiscard]]  virtual       std::string::iterator ToStringBuffer(std::string&) const = 0;
    [[nodiscard]]  virtual       std::string::iterator OnToString(std::string::iterator, std::string::iterator) const = 0;

                   virtual                             ~ISerializer() = default;
};
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------




}}  /*END OF NAMESPACES*/




#endif // _NICKSV_CHAT_ISERIALIZER_T