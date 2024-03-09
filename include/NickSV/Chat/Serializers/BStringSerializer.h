#ifndef _NICKSV_CHAT_BSTRING_SERIALIZER_HEADER
#define _NICKSV_CHAT_BSTRING_SERIALIZER_HEADER
#pragma once


#include <string>

#include "NickSV/Chat/Interfaces/ISerializer.h"
#include "NickSV/Chat/Utils.h"


namespace NickSV::Chat {



template<typename CharT>
class Serializer<std::basic_string<CharT>> final : public ISerializer
{
    /*
    Class helper handles the serialization of std::basic_string struct to string 
    and parsing (deserializing) it back.

    Inherit from this class to handle its functions on your custom 
    std::basic_string's child class with your extra info

    */
public:
    using CharType = CharT;

    Serializer() = delete;
    explicit Serializer(const std::basic_string<CharT>* const);

    inline const std::basic_string<CharT>* const GetObject() const;

    size_t GetSize() const override;
    size_t OnGetSize(size_t baseSize = 0) const override;

    std::string ToString() const override;
    std::string::iterator ToString(std::string::iterator, std::string::iterator) const override;
    std::string::iterator OnToString(std::string::iterator, std::string::iterator) const override;
    std::string::iterator ToStringBuffer(std::string&) const override;
private:
    /*
    Pointer to std::basic_string object where "this" serializes from or parses to.
    */
    const std::basic_string<CharT>* const m_cpcBasicString; //FIXME maybe const is redundant
};


} /*END OF NAMESPACES*/



#endif // _NICKSV_CHAT_BSTRING_SERIALIZER_HEADER