
#ifndef _NICKSV_CHAT_BMESSAGE_SERIALIZER_PASTE
#define _NICKSV_CHAT_BMESSAGE_SERIALIZER_PASTE
#pragma once



#include "NickSV/Chat/Defines.h"
#include "NickSV/Chat/Utils.h"
#include "NickSV/Chat/BasicMessage.h"
#include "NickSV/Chat/Serializers/BStringSerializer.h"
#include "NickSV/Chat/Serializers/BMessageSerializer.h"


namespace NickSV::Chat {






//----------------------------------------------------------------------------------------------------
// Serializer<BasicMessage<CharT>> implementation
//----------------------------------------------------------------------------------------------------
template<typename CharT>
Serializer<BasicMessage<CharT>>::Serializer(BasicMessage<CharT>* pBasicMessage) : m_cpBasicMessage(pBasicMessage)
{ 
    CHAT_ASSERT(m_cpBasicMessage, "m_cpBasicMessage must not be nullptr");
};

template<typename CharT>
inline size_t Serializer<BasicMessage<CharT>>::GetSize() const
{
    size_t size = 
      sizeof(m_cpBasicMessage->m_sText.size()) +
      sizeof(CharType) * m_cpBasicMessage->m_sText.size();
    return size + OnGetSize(size);
}

template<typename CharT>
inline size_t Serializer<BasicMessage<CharT>>::OnGetSize(size_t baseSize) const { return 0; }

template<typename CharT>
std::string Serializer<BasicMessage<CharT>>::ToString() const
{
    std::string str;
    str.resize(GetSize());
    ToStringBuffer(str);
    return str;
}

template<typename CharT>
std::string::iterator Serializer<BasicMessage<CharT>>::ToString(std::string::iterator begin, std::string::iterator end) const
{
    CHAT_ASSERT(end >= begin + GetSize(), invalid_range_size_ERROR_MESSAGE);
    auto iter = ConstSerializer<std::basic_string<CharT>>(&m_cpBasicMessage->m_sText).ToString(begin, end);
    CHAT_ASSERT(iter <= end, something_went_wrong_ERROR_MESSAGE);
    return OnToString(iter, end);
}

template<typename CharT>
inline std::string::iterator Serializer<BasicMessage<CharT>>::OnToString(std::string::iterator begin, std::string::iterator end) const
{
    return begin;
}

template<typename CharT>
inline std::string::iterator Serializer<BasicMessage<CharT>>::ToStringBuffer(std::string& buffer) const
{
    return ToString(buffer.begin(), buffer.end());
}

template<typename CharT>
inline std::string::const_iterator Serializer<BasicMessage<CharT>>::ParseFromString(const std::string& str)
{
    return ParseFromString(str.cbegin(), str.cend());
}

template<typename CharT>
std::string::const_iterator Serializer<BasicMessage<CharT>>::ParseFromString(std::string::const_iterator begin, std::string::const_iterator end)
{
    auto iter = Serializer<std::basic_string<CharT>>(&m_cpBasicMessage->m_sText).ParseFromString(begin, end);
    if(iter <= begin)
        return begin; //BAD INPUT. Parsed size doesnt match with input size

    return OnParseFromString(iter, end);
}

template<typename CharT>
std::string::const_iterator inline Serializer<BasicMessage<CharT>>::OnParseFromString(std::string::const_iterator begin, std::string::const_iterator end)
{ 
    return begin; 
}
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------






//----------------------------------------------------------------------------------------------------
// ConstSerializer<BasicMessage<CharT>> implementation
//----------------------------------------------------------------------------------------------------
template<typename CharT>
ConstSerializer<BasicMessage<CharT>>::ConstSerializer(const BasicMessage<CharT>* pBasicMessage) : m_cpcBasicMessage(pBasicMessage)
{ 
    CHAT_ASSERT(m_cpcBasicMessage, "m_cpcBasicMessage must not be nullptr");
};

template<typename CharT>
inline size_t ConstSerializer<BasicMessage<CharT>>::GetSize() const
{
    size_t size =
      sizeof(m_cpcBasicMessage->m_sText.size()) +
      sizeof(CharType) * m_cpcBasicMessage->m_sText.size();
    return size + OnGetSize(size);
}

template<typename CharT>
inline size_t ConstSerializer<BasicMessage<CharT>>::OnGetSize(size_t baseSize) const { return 0; }

template<typename CharT>
std::string ConstSerializer<BasicMessage<CharT>>::ToString() const
{
    std::string str;
    str.resize(GetSize());
    ToStringBuffer(str);
    return str;
}

template<typename CharT>
std::string::iterator ConstSerializer<BasicMessage<CharT>>::ToString(std::string::iterator begin, std::string::iterator end) const
{
    CHAT_ASSERT(end >= begin + GetSize(), invalid_range_size_ERROR_MESSAGE);
    auto iter = ConstSerializer<std::basic_string<CharT>>(&m_cpcBasicMessage->m_sText).ToString(begin, end);
    CHAT_ASSERT(iter <= end, something_went_wrong_ERROR_MESSAGE);
    return OnToString(iter, end);
}

template<typename CharT>
inline std::string::iterator ConstSerializer<BasicMessage<CharT>>::OnToString(std::string::iterator begin, std::string::iterator end) const
{
    return begin;
}

template<typename CharT>
inline std::string::iterator ConstSerializer<BasicMessage<CharT>>::ToStringBuffer(std::string& buffer) const
{
    return ToString(buffer.begin(), buffer.end());
}


template<typename CharT>
inline std::string::const_iterator ConstSerializer<BasicMessage<CharT>>::ParseFromString(std::string::const_iterator begin, std::string::const_iterator end)
{ 
    CHAT_EXPECT(false, const_class_ERROR_MESSAGE);
    return begin;
}

template<typename CharT>
inline std::string::const_iterator ConstSerializer<BasicMessage<CharT>>::ParseFromString(const std::string& str)
{ 
    CHAT_EXPECT(false, const_class_ERROR_MESSAGE);
    return str.begin();
}

template<typename CharT>
inline std::string::const_iterator ConstSerializer<BasicMessage<CharT>>::OnParseFromString(std::string::const_iterator begin, std::string::const_iterator end)
{
    CHAT_EXPECT(false, const_class_ERROR_MESSAGE);
    return begin;
}
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------


template class Serializer<BasicMessage<char>>;
template class Serializer<BasicMessage<wchar_t>>;
template class Serializer<BasicMessage<char16_t>>;
template class Serializer<BasicMessage<char32_t>>;

template class ConstSerializer<BasicMessage<char>>;
template class ConstSerializer<BasicMessage<wchar_t>>;
template class ConstSerializer<BasicMessage<char16_t>>;
template class ConstSerializer<BasicMessage<char32_t>>;


} /*END OF NAMESPACES*/



#endif // _NICKSV_CHAT_BMESSAGE_SERIALIZER_PASTE