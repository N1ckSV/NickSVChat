#ifndef _NICKSV_CHAT_BMESSAGE_SERIALIZER_HEADER
#define _NICKSV_CHAT_BMESSAGE_SERIALIZER_HEADER
#pragma once


#include <string>

#include "NickSV/Chat/Interfaces/ISerializer.h"
#include "NickSV/Chat/Utils.h"


namespace NickSV::Chat {





template<typename CharT>
class Serializer<BasicMessage<CharT>> : public ISerializer
{
    /*
    Class helper handles the serialization of BasicMessage struct to string 
    and parsing (deserializing) it back.

    Inherit from this class to handle its functions on your custom 
    BasicMessage's child class with your extra info

    */
    static_assert(is_serializable<BasicMessage<CharT>>::value, is_serializable_ERROR_MESSAGE);
public:
    using CharType = CharT;

    Serializer() = delete;
    explicit Serializer(BasicMessage<CharT>*);

    inline size_t GetSize() const override final;
    inline size_t OnGetSize(size_t baseSize = 0) const override;

    std::string ToString() const override final;
    std::string::iterator ToString(std::string::iterator, std::string::iterator) const override final;
    inline std::string::iterator OnToString(std::string::iterator, std::string::iterator) const override;
    inline std::string::iterator ToStringBuffer(std::string&) const override final;

    inline std::string::const_iterator ParseFromString(const std::string&) override final;
           std::string::const_iterator ParseFromString(std::string::const_iterator, std::string::const_iterator) override final;
    inline std::string::const_iterator OnParseFromString(std::string::const_iterator, std::string::const_iterator) override;
private:
    /*
    Pointer to BasicMessage object where "this" serializes from or parses to.
    */
    BasicMessage<CharT>* const m_cpBasicMessage; //FIXME maybe const is redundant
};

template<typename CharT>
class ConstSerializer<BasicMessage<CharT>> : public ISerializer
{
    /*
    Same as Serializer<BasicMessage<CharT>> but const
    */
    static_assert(is_serializable<BasicMessage<CharT>>::value, is_serializable_ERROR_MESSAGE);
public:
    using CharType = typename BasicMessage<CharT>::CharType;

    ConstSerializer() = delete;
    explicit ConstSerializer(const BasicMessage<CharT>*);
    inline size_t GetSize() const override final;
    inline size_t OnGetSize(size_t baseSize = 0) const override;
    
    std::string ToString() const override final;
    std::string::iterator ToString(std::string::iterator, std::string::iterator) const override final;
    inline std::string::iterator OnToString(std::string::iterator, std::string::iterator) const override;
    inline std::string::iterator ToStringBuffer(std::string&) const override final;

    inline std::string::const_iterator ParseFromString(const std::string&) override final;
    inline std::string::const_iterator ParseFromString(std::string::const_iterator, std::string::const_iterator) override final;
    inline std::string::const_iterator OnParseFromString(std::string::const_iterator, std::string::const_iterator) override final;
private:
    const BasicMessage<CharT>* const m_cpcBasicMessage; //FIXME maybe second const is redundant
};



} /*END OF NAMESPACES*/



#endif // _NICKSV_CHAT_BMESSAGE_SERIALIZER_HEADER