
#ifndef _NICKSV_CHAT_BCLIENTINFO_SERIALIZER_PASTE
#define _NICKSV_CHAT_BCLIENTINFO_SERIALIZER_PASTE
#pragma once


#include "NickSV/Chat/Defines.h"
#include "NickSV/Chat/Utils.h"
#include "NickSV/Chat/BasicClientInfo.h"
#include "NickSV/Chat/Serializers/BStringSerializer.h"
#include "NickSV/Chat/Serializers/BClientInfoSerializer.h"


namespace NickSV::Chat {






//----------------------------------------------------------------------------------------------------
// Serializer<BasicClientInfo<CharT>> implementation
//----------------------------------------------------------------------------------------------------
template<typename CharT>
Serializer<BasicClientInfo<CharT>>::Serializer(BasicClientInfo<CharT>* pBasicClientInfo) : m_cpBasicClientInfo(pBasicClientInfo)
{ 
    CHAT_ASSERT(m_cpBasicClientInfo, "m_cpBasicClientInfo must not be nullptr");
};

template<typename CharT>
inline size_t Serializer<BasicClientInfo<CharT>>::GetSize() const
{
    size_t size = 
      sizeof(APIVersionType) +
      sizeof(m_cpBasicClientInfo->m_sNick.size()) +
      sizeof(CharType) * m_cpBasicClientInfo->m_sNick.size();
    return size + OnGetSize(size);
}

template<typename CharT>
inline size_t Serializer<BasicClientInfo<CharT>>::OnGetSize(size_t baseSize) const { return 0; }

template<typename CharT>
std::string Serializer<BasicClientInfo<CharT>>::ToString() const
{
    std::string str;
    str.resize(GetSize());
    ToStringBuffer(str);
    return str;
}

template<typename CharT>
std::string::iterator Serializer<BasicClientInfo<CharT>>::ToString(std::string::iterator begin, std::string::iterator end) const
{
    CHAT_ASSERT(end >= begin + GetSize(), invalid_range_size_ERROR_MESSAGE);
    auto verChar = reinterpret_cast<const char*>(&m_cpBasicClientInfo->m_nAPIVer);
    std::string::iterator iter = std::copy(verChar, verChar + sizeof(APIVersionType), begin);
    iter = ConstSerializer<std::basic_string<CharT>>(&m_cpBasicClientInfo->m_sNick).ToString(iter, end);
    CHAT_ASSERT(iter <= end, something_went_wrong_ERROR_MESSAGE);
    return OnToString(iter, end);
}

template<typename CharT>
inline std::string::iterator Serializer<BasicClientInfo<CharT>>::OnToString(std::string::iterator begin, std::string::iterator end) const
{
    return begin;
}

template<typename CharT>
inline std::string::iterator Serializer<BasicClientInfo<CharT>>::ToStringBuffer(std::string& buffer) const
{
    return ToString(buffer.begin(), buffer.end());
}

template<typename CharT>
inline std::string::const_iterator Serializer<BasicClientInfo<CharT>>::ParseFromString(const std::string& str)
{
    return ParseFromString(str.cbegin(), str.cend());
}

template<typename CharT>
std::string::const_iterator Serializer<BasicClientInfo<CharT>>::ParseFromString(std::string::const_iterator begin, std::string::const_iterator end)
{
    if(begin + sizeof(APIVersionType) + sizeof(size_t) > end)
        return begin; //BAD INPUT. Range size has to be at least sizeof(APIVersionType) + sizeof(size_t) bytes long

    std::copy(begin, begin + sizeof(APIVersionType), reinterpret_cast<char*>(&m_cpBasicClientInfo->m_nAPIVer));
    auto iter = begin + sizeof(APIVersionType);
    auto newIter = Serializer<std::basic_string<CharT>>(&m_cpBasicClientInfo->m_sNick).ParseFromString(iter, end);
    if(newIter <= iter)
        return begin; //BAD INPUT. Parsed size doesnt match with input size

    return OnParseFromString(newIter, end);
}

template<typename CharT>
std::string::const_iterator inline Serializer<BasicClientInfo<CharT>>::OnParseFromString(std::string::const_iterator begin, std::string::const_iterator end)
{ 
    return begin; 
}
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------






//----------------------------------------------------------------------------------------------------
// ConstSerializer<BasicClientInfo<CharT>> implementation
//----------------------------------------------------------------------------------------------------
template<typename CharT>
ConstSerializer<BasicClientInfo<CharT>>::ConstSerializer(const BasicClientInfo<CharT>* pBasicClientInfo) : m_cpcBasicClientInfo(pBasicClientInfo)
{ 
    CHAT_ASSERT(m_cpcBasicClientInfo, "m_cpcBasicClientInfo must not be nullptr");
};

template<typename CharT>
inline size_t ConstSerializer<BasicClientInfo<CharT>>::GetSize() const
{
    size_t size = 
      sizeof(APIVersionType) +
      sizeof(m_cpcBasicClientInfo->m_sNick.size()) +
      sizeof(CharType) * m_cpcBasicClientInfo->m_sNick.size();
    return size + OnGetSize(size);
}

template<typename CharT>
inline size_t ConstSerializer<BasicClientInfo<CharT>>::OnGetSize(size_t baseSize) const { return 0; }

template<typename CharT>
std::string ConstSerializer<BasicClientInfo<CharT>>::ToString() const
{
    std::string str;
    str.resize(GetSize());
    ToStringBuffer(str);
    return str;
}

template<typename CharT>
std::string::iterator ConstSerializer<BasicClientInfo<CharT>>::ToString(std::string::iterator begin, std::string::iterator end) const
{
    CHAT_ASSERT(end >= begin + GetSize(), invalid_range_size_ERROR_MESSAGE);
    auto verChar = reinterpret_cast<const char*>(&m_cpcBasicClientInfo->m_nAPIVer);
    std::string::iterator iter = std::copy(verChar, verChar + sizeof(APIVersionType), begin);
    iter = ConstSerializer<std::basic_string<CharT>>(&m_cpcBasicClientInfo->m_sNick).ToString(iter, end);
    CHAT_ASSERT(iter <= end, something_went_wrong_ERROR_MESSAGE);
    return OnToString(iter, end);
}

template<typename CharT>
inline std::string::iterator ConstSerializer<BasicClientInfo<CharT>>::OnToString(std::string::iterator begin, std::string::iterator end) const
{
    return begin;
}

template<typename CharT>
inline std::string::iterator ConstSerializer<BasicClientInfo<CharT>>::ToStringBuffer(std::string& buffer) const
{
    return ToString(buffer.begin(), buffer.end());
}


template<typename CharT>
inline std::string::const_iterator ConstSerializer<BasicClientInfo<CharT>>::ParseFromString(std::string::const_iterator begin, std::string::const_iterator end)
{ 
    CHAT_EXPECT(false, const_class_ERROR_MESSAGE);
    return begin;
}

template<typename CharT>
inline std::string::const_iterator ConstSerializer<BasicClientInfo<CharT>>::ParseFromString(const std::string& str)
{ 
    CHAT_EXPECT(false, const_class_ERROR_MESSAGE);
    return str.begin();
}

template<typename CharT>
inline std::string::const_iterator ConstSerializer<BasicClientInfo<CharT>>::OnParseFromString(std::string::const_iterator begin, std::string::const_iterator end)
{
    CHAT_EXPECT(false, const_class_ERROR_MESSAGE);
    return begin;
}
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------



template class Serializer<BasicClientInfo<char>>;
template class Serializer<BasicClientInfo<wchar_t>>;
template class Serializer<BasicClientInfo<char16_t>>;
template class Serializer<BasicClientInfo<char32_t>>;

template class ConstSerializer<BasicClientInfo<char>>;
template class ConstSerializer<BasicClientInfo<wchar_t>>;
template class ConstSerializer<BasicClientInfo<char16_t>>;
template class ConstSerializer<BasicClientInfo<char32_t>>;



} /*END OF NAMESPACES*/



#endif // _NICKSV_CHAT_BCLIENTINFO_SERIALIZER_PASTE