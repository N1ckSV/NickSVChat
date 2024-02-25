#ifndef _NICKSV_CHAT_BCLIENTINFO_SERIALIZER_HEADER
#define _NICKSV_CHAT_BCLIENTINFO_SERIALIZER_HEADER
#pragma once


#include <string>

#include "NickSV/Chat/Interfaces/ISerializer.h"
#include "NickSV/Chat/Utils.h"


namespace NickSV::Chat {



template<typename CharT>
class Serializer<BasicClientInfo<CharT>> : public ISerializer
{
    /*

    ALIASES: ClientInfoSerializer<>

    Class helper handles the serialization of BasicClientInfo struct to string 
    and parsing (deserializing) it back.

    Inherit from this class to handle its functions on your custom 
    BasicClientInfo's child class with your extra info

    */
    static_assert(is_serializable<BasicClientInfo<CharT>>::value, is_serializable_ERROR_MESSAGE);
public:
    using CharType = typename BasicClientInfo<CharT>::CharType;
    using APIVersionType = typename BasicClientInfo<CharType>::APIVersionType;

    Serializer() = delete;
    explicit Serializer(BasicClientInfo<CharT>*);

    /*
    Returns the size in bytes of BasicClientInfo serialization into std::string.
    Used in ToString() and ToStringBuffer(...).
    See OnGetSize(...) to interact:
    Derived::GetSize() == Base::GetSize() + Derived::OnGetSize()
    */
    inline size_t GetSize() const override final;

    /*
    Override it in your derived class. Supposed to return an extra
    info size for serializing.
    Derived::GetSize() == Base::GetSize() + Derived::OnGetSize()
    */
    inline size_t OnGetSize(size_t baseSize = 0) const override;
    
    /*
    Serializes BasicClientInfo to std::string which can be converted back deterministically.
    See OnToString(...) to interact
    */
    std::string ToString() const override final;

    /*
    FIXME
    */
    std::string::iterator ToString(std::string::iterator,std::string::iterator) const override final;

    /*
    Override it in your derived class and add an extra info.
    pvNewPartStart is supposed to be a pointer to an end of
    serialized BasicClientInfo string output part.
    Use pvNewPartStart to add an extra info.
    This function is called inside ToString() and ToStringBuffer(...)
    */
    inline std::string::iterator OnToString(std::string::iterator, std::string::iterator) const override;

    /*
    Same as ToString() but with externally predefined std::string buffer.
    See OnToString(...) to interact
    */
    inline std::string::iterator ToStringBuffer(std::string&) const override final;

    /*
    Parses std::string to BasicClientInfo (m_pBasicClientInfo).
    See OnParseFromString(...) to interact.
    Return values:
        EResult::Success - All good and m_pBasicClientInfo points to
                           a valid BasicClientInfo or its child value
        EResult::Error  - Something went wrong and m_pBasicClientInfo 
                           points to an unspecified BasicClientInfo or its child value
                           (some member values could be parsed for debugging)
        OnParseFromString(...) - Returned if all BasicClientInfo member values
                                 is parsed and valid.
                                 Returns EResult::Success by default.
    */
    inline std::string::const_iterator ParseFromString(const std::string&) override final;
    std::string::const_iterator ParseFromString(std::string::const_iterator, std::string::const_iterator) override final;

    /*
    Override it in your derived class to parse an extra info.
    pcvNewPartStart is supposed to be a pointer to an end of 
    serialized BasicClientInfo string input part.
    Use pcvNewPartStart to parse an extra info.
    This function is called inside ParseFromString(...) 
    and MakeFromString(...).
    Returns just EResult::Success by default.
    */
    inline std::string::const_iterator OnParseFromString(std::string::const_iterator, std::string::const_iterator) override;
private:
    /*
    Pointer to BasicClientInfo object where "this" serializes from or parses to.
    */
    BasicClientInfo<CharType>* const m_cpBasicClientInfo; //FIXME maybe const is redundant
};

template<typename CharT>
class ConstSerializer<BasicClientInfo<CharT>> : public ISerializer
{
    /*
    Same as Serializer<BasicClientInfo<CharT>> but const
    */
    static_assert(is_serializable<BasicClientInfo<CharT>>::value, is_serializable_ERROR_MESSAGE);
public:
    using CharType = typename BasicClientInfo<CharT>::CharType;
    using APIVersionType = typename BasicClientInfo<CharType>::APIVersionType;

    ConstSerializer() = delete;
    explicit ConstSerializer(const BasicClientInfo<CharT>*);

    inline size_t GetSize() const override final;
    inline size_t OnGetSize(size_t baseSize = 0) const override;

    std::string ToString() const override final;
    std::string::iterator ToString(std::string::iterator,std::string::iterator) const override final;
    inline std::string::iterator OnToString(std::string::iterator, std::string::iterator) const override;
    inline std::string::iterator ToStringBuffer(std::string&) const override final;

    inline std::string::const_iterator ParseFromString(const std::string&) override final;
    inline std::string::const_iterator ParseFromString(std::string::const_iterator, std::string::const_iterator) override final;
    inline std::string::const_iterator OnParseFromString(std::string::const_iterator, std::string::const_iterator) override final;
private:
    const BasicClientInfo<CharType>* const m_cpcBasicClientInfo; //FIXME maybe second const is redundant
};




} /*END OF NAMESPACES*/



#endif // _NICKSV_CHAT_BCLIENTINFO_SERIALIZER_HEADER