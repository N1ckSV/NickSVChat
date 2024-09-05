#ifndef _NICKSV_CHAT_MESSAGE_SERIALIZER_HEADER
#define _NICKSV_CHAT_MESSAGE_SERIALIZER_HEADER
#pragma once


#include <string>

#include "NickSV/Chat/Interfaces/ISerializer.h"
#include "NickSV/Chat/Message.h"


namespace NickSV {
namespace Chat {




template<>
class NICKSVCHAT_API Serializer<Message> : public ISerializer
{
    /*
    Class helper handles the serialization of Message struct to string 
    and parsing (deserializing) it back.

    Inherit from this class to handle its functions on your custom 
    Message's child class with your extra info

    */
public:

    Serializer() = delete;
    explicit Serializer(const Message&);

    inline const Message& GetObject() const;

    size_t GetSize() const override final;
    size_t OnGetSize(size_t baseSize = 0) const override;

    std::string ToString() const override final;
    std::string::iterator ToString(std::string::iterator, std::string::iterator) const override final;
    std::string::iterator OnToString(std::string::iterator, std::string::iterator) const override;
    std::string::iterator ToStringBuffer(std::string&) const override final;
private:
    /*
    Pointer to Message object where "this" serializes from or parses to.
    */
    const Message& m_rMessage;
};



}}  /*END OF NAMESPACES*/



#endif // _NICKSV_CHAT_BMESSAGE_SERIALIZER_HEADER