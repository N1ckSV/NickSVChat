
#ifndef _NICKSV_CHAT_ISERIALIZER_T
#define _NICKSV_CHAT_ISERIALIZER_T
#pragma once


#include <string>

#include "NickSV/Chat/Types.h"




namespace NickSV::Chat {




//------------------------------------------------------------------------------------
// Serializer interface
//------------------------------------------------------------------------------------
class ISerializer
{
public:
    CHAT_NODISCARD virtual inline               size_t GetSize() const = 0;
    CHAT_NODISCARD virtual inline               size_t OnGetSize(size_t baseSize = 0) const = 0;

    CHAT_NODISCARD virtual                 std::string ToString() const = 0;
    CHAT_NODISCARD virtual       std::string::iterator ToString(std::string::iterator, std::string::iterator) const = 0;
    CHAT_NODISCARD virtual       std::string::iterator ToStringBuffer(std::string&) const = 0;
    CHAT_NODISCARD virtual       std::string::iterator OnToString(std::string::iterator, std::string::iterator) const = 0;

                   virtual                             ~ISerializer() = default;
};
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------




} /*END OF NAMESPACES*/




#endif // _NICKSV_CHAT_ISERIALIZER_T