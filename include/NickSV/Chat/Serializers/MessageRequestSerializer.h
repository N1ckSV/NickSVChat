#ifndef _NICKSV_CHAT_MESSAGE_REQUEST_SERIALIZER_HEADER
#define _NICKSV_CHAT_MESSAGE_REQUEST_SERIALIZER_HEADER
#pragma once


#include <string>

#include "NickSV/Chat/Interfaces/ISerializer.h"
#include "NickSV/Chat/Requests/MessageRequest.h"
#include "NickSV/Chat/Utils.h"


namespace NickSV {
namespace Chat {





template<>
class Serializer<MessageRequest> : public ISerializer
{
    /*
    Class helper handles the serialization of BasicMessage struct to string 
    and parsing (deserializing) it back.

    Inherit from this class to handle its functions on your custom 
    BasicMessage's child class with your extra info

    */
   static_assert(is_serializable<Message>::value, is_serializable_ERROR_MESSAGE);
public:
    Serializer() = delete;
    explicit Serializer(const MessageRequest* const);

    inline const MessageRequest* GetObject() const;

    inline size_t GetSize() const override final;
    inline size_t OnGetSize(size_t baseSize = 0) const override;

    std::string ToString() const override final;
    std::string::iterator ToString(std::string::iterator, std::string::iterator) const override final;
    inline std::string::iterator OnToString(std::string::iterator, std::string::iterator) const override;
    inline std::string::iterator ToStringBuffer(std::string&) const override final;
private:
    /*
    Pointer to BasicMessage object where "this" serializes from or parses to.
    */
    const MessageRequest* const m_cpcMessageRequest; //FIXME maybe const is redundant
};



}}  /*END OF NAMESPACES*/



#endif // _NICKSV_CHAT_MESSAGE_REQUEST_SERIALIZER_HEADER