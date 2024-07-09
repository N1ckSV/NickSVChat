#ifndef _NICKSV_CHAT_BMESSAGE_SERIALIZER_HEADER
#define _NICKSV_CHAT_BMESSAGE_SERIALIZER_HEADER
#pragma once


#include <string>

#include "NickSV/Chat/Interfaces/ISerializer.h"
#include "NickSV/Chat/BasicMessage.h"
#include "NickSV/Chat/Utils.h"


namespace NickSV {
namespace Chat {





template<typename CharT>
class Serializer<BasicMessage<CharT>> : public ISerializer
{
    /*
    Class helper handles the serialization of BasicMessage struct to string 
    and parsing (deserializing) it back.

    Inherit from this class to handle its functions on your custom 
    BasicMessage's child class with your extra info

    */
public:
    using CharType = CharT;

    Serializer() = delete;
    explicit Serializer(const BasicMessage<CharT>* const);

    inline const BasicMessage<CharT>* GetObject() const;

    size_t GetSize() const override final;
    size_t OnGetSize(size_t baseSize = 0) const override;

    std::string ToString() const override final;
    std::string::iterator ToString(std::string::iterator, std::string::iterator) const override final;
    std::string::iterator OnToString(std::string::iterator, std::string::iterator) const override;
    std::string::iterator ToStringBuffer(std::string&) const override final;
private:
    /*
    Pointer to BasicMessage object where "this" serializes from or parses to.
    */
    const BasicMessage<CharT>* const m_cpcBasicMessage; //FIXME maybe const is redundant
};



}}  /*END OF NAMESPACES*/



#endif // _NICKSV_CHAT_BMESSAGE_SERIALIZER_HEADER