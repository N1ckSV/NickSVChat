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
public:
    using CharType = CharT;

    Serializer() = delete;
    explicit Serializer(std::basic_string<CharT>*);

    inline size_t GetSize() const override;
    inline size_t OnGetSize(size_t baseSize) const override;

    std::string ToString() const override;
    std::string::iterator ToString(std::string::iterator, std::string::iterator) const override;
    inline std::string::iterator OnToString(std::string::iterator, std::string::iterator) const override;
    inline std::string::iterator ToStringBuffer(std::string&) const override;

    inline std::string::const_iterator ParseFromString(const std::string&) override;
           std::string::const_iterator ParseFromString(std::string::const_iterator, std::string::const_iterator) override;
    inline std::string::const_iterator OnParseFromString(std::string::const_iterator, std::string::const_iterator) override;
private:
    std::basic_string<CharT>* const m_cpBasicString; //FIXME maybe const is redundant
};

template<typename CharT>
class ConstSerializer<std::basic_string<CharT>> final : public ISerializer
{
public:
    using CharType = CharT;

    ConstSerializer() = delete;
    explicit ConstSerializer(const std::basic_string<CharT>*);

    inline size_t GetSize() const override;
    inline size_t OnGetSize(size_t) const override;
    
    std::string ToString() const override;
    std::string::iterator ToString(std::string::iterator, std::string::iterator) const override;
    inline std::string::iterator OnToString(std::string::iterator, std::string::iterator) const override;
    inline std::string::iterator ToStringBuffer(std::string&) const override;

    inline std::string::const_iterator ParseFromString(const std::string&) override;
    inline std::string::const_iterator ParseFromString(std::string::const_iterator, std::string::const_iterator) override;
    inline std::string::const_iterator OnParseFromString(std::string::const_iterator, std::string::const_iterator) override;
private:
    const std::basic_string<CharT>* const m_cpcBasicString; //FIXME maybe second const is redundant
};



} /*END OF NAMESPACES*/



#endif // _NICKSV_CHAT_BSTRING_SERIALIZER_HEADER