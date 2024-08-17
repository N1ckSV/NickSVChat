
#ifndef _NICKSV_CHAT_ISERIALIZABLE_T
#define _NICKSV_CHAT_ISERIALIZABLE_T
#pragma once


#include <memory>


#include "NickSV/Chat/Types.h"


#include "NickSV/Chat/Interfaces/IParser.h"




namespace NickSV {
namespace Chat {






//------------------------------------------------------------------------------------
// Interface helper for structs, if you need to make them serializable.
//------------------------------------------------------------------------------------
class NICKSVCHAT_API ISerializable
{
public:
    [[nodiscard]]  virtual const std::unique_ptr<ISerializer>        GetSerializer() const = 0;
                   virtual                                           ~ISerializable() = default;
};
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------




}}  /*END OF NAMESPACES*/




#endif // _NICKSV_CHAT_ISERIALIZABLE_T