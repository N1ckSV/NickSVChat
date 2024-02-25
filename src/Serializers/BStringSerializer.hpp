
#ifndef _NICKSV_CHAT_BSTRING_SERIALIZER_PASTE
#define _NICKSV_CHAT_BSTRING_SERIALIZER_PASTE
#pragma once




#include "NickSV/Chat/Defines.h"
#include "NickSV/Chat/Utils.h"
#include "NickSV/Chat/Serializers/BStringSerializer.h"


namespace NickSV::Chat {






//----------------------------------------------------------------------------------------------------
// Serializer<std::basic_string<CharT>> implementation
//----------------------------------------------------------------------------------------------------
template<typename CharT>
Serializer<std::basic_string<CharT>>::Serializer(std::basic_string<CharT>* pBasicString) : m_cpBasicString(pBasicString)
{ 
    CHAT_ASSERT(m_cpBasicString, "m_cpBasicString must not be nullptr");
};

template<typename CharT>
inline size_t Serializer<std::basic_string<CharT>>::GetSize() const
{
    size_t size =
      sizeof(m_cpBasicString->size()) +
      sizeof(CharType) * m_cpBasicString->size();
    return size;
}

template<typename CharT>
inline size_t Serializer<std::basic_string<CharT>>::OnGetSize(size_t dummy) const
{
    return 0;
}

template<typename CharT>
std::string Serializer<std::basic_string<CharT>>::ToString() const
{
    std::string str;
    str.resize(GetSize());
    ToStringBuffer(str);
    return str;
}

template<typename CharT>
std::string::iterator Serializer<std::basic_string<CharT>>::ToString(std::string::iterator begin, std::string::iterator end) const
{
    CHAT_ASSERT(end >= begin + GetSize(), "Range size must be greater or equal than serialization size and preallocated by you");
    size_t size = m_cpBasicString->size();
    auto sizeChar = reinterpret_cast<const char*>(&size);
    auto iter = std::copy(sizeChar, sizeChar + sizeof(size_t), begin);
    iter = std::copy(reinterpret_cast<const char*>(m_cpBasicString->cbegin().base()),
                     reinterpret_cast<const char*>(m_cpBasicString->cend().base()), iter);
    CHAT_ASSERT(iter <= end, something_went_wrong_ERROR_MESSAGE);
    return iter;
}

template<typename CharT>
inline std::string::iterator Serializer<std::basic_string<CharT>>::OnToString(std::string::iterator begin, std::string::iterator end) const
{
    return begin;
}

template<typename CharT>
inline std::string::iterator Serializer<std::basic_string<CharT>>::ToStringBuffer(std::string& buffer) const
{
    return ToString(buffer.begin(), buffer.end());
}

template<typename CharT>
inline std::string::const_iterator Serializer<std::basic_string<CharT>>::ParseFromString(const std::string& str) 
{
    return ParseFromString(str.cbegin(), str.cend());
}

template<typename CharT>
std::string::const_iterator Serializer<std::basic_string<CharT>>::ParseFromString(std::string::const_iterator begin, std::string::const_iterator end) 
{
    if(begin + sizeof(size_t) > end)
        return begin; //BAD INPUT. Range size has to be at least sizeof(APIVersionType) + sizeof(size_t) bytes long

    size_t textSize = 0;
    std::copy(begin, begin + sizeof(size_t), reinterpret_cast<char*>(&textSize));
    auto iter = begin + sizeof(size_t);
    if(iter + sizeof(CharType) * textSize > end)
        return begin; //BAD INPUT. Range size has to be at least sizeof(APIVersionType) + sizeof(size_t) bytes long
    
    m_cpBasicString->resize(textSize);
    std::copy(iter, iter + sizeof(CharType) * textSize, reinterpret_cast<char*>(m_cpBasicString->data()));
    iter += sizeof(CharType) * textSize;
    return iter;
}

template<typename CharT>
inline std::string::const_iterator Serializer<std::basic_string<CharT>>::OnParseFromString(std::string::const_iterator begin, std::string::const_iterator end) 
{ 
    return begin; 
};
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------






//----------------------------------------------------------------------------------------------------
// ConstSerializer<std::basic_string<CharT>> implementation
//----------------------------------------------------------------------------------------------------
template<typename CharT>
ConstSerializer<std::basic_string<CharT>>::ConstSerializer(const std::basic_string<CharT>* pcBasicString) : m_cpcBasicString(pcBasicString)
{ 
    CHAT_ASSERT(m_cpcBasicString, "m_cpcBasicString must not be nullptr");
};

template<typename CharT>
inline size_t ConstSerializer<std::basic_string<CharT>>::GetSize() const
{
    size_t size =
      sizeof(m_cpcBasicString->size()) +
      sizeof(CharType) * m_cpcBasicString->size();
    return size;
}

template<typename CharT>
inline size_t ConstSerializer<std::basic_string<CharT>>::OnGetSize(size_t dummy) const { return 0; };

template<typename CharT>
std::string ConstSerializer<std::basic_string<CharT>>::ToString() const
{
    std::string str;
    str.resize(GetSize());
    ToStringBuffer(str);
    return str;
}

template<typename CharT>
std::string::iterator ConstSerializer<std::basic_string<CharT>>::ToString(std::string::iterator begin, std::string::iterator end) const
{
    CHAT_ASSERT(end >= begin + GetSize(), "Range size must be greater or equal than serialization size and preallocated by you");
    size_t size = m_cpcBasicString->size();
    auto sizeChar = reinterpret_cast<const char*>(&size);
    auto iter = std::copy(sizeChar, sizeChar + sizeof(size_t), begin);
    iter = std::copy(reinterpret_cast<const char*>(m_cpcBasicString->cbegin().base()),
                     reinterpret_cast<const char*>(m_cpcBasicString->cend().base()), iter);
    CHAT_ASSERT(iter <= end, something_went_wrong_ERROR_MESSAGE);
    return iter;
}

template<typename CharT>
std::string::iterator ConstSerializer<std::basic_string<CharT>>::OnToString(std::string::iterator begin, std::string::iterator end) const
{
    return begin;
}

template<typename CharT>
inline std::string::iterator ConstSerializer<std::basic_string<CharT>>::ToStringBuffer(std::string& buffer) const
{
    return ToString(buffer.begin(), buffer.end());
}

template<typename CharT>
inline std::string::const_iterator ConstSerializer<std::basic_string<CharT>>::ParseFromString(const std::string& str)
{ 
    CHAT_EXPECT(false, const_class_ERROR_MESSAGE);
    return str.begin();
}

template<typename CharT>
inline std::string::const_iterator ConstSerializer<std::basic_string<CharT>>::ParseFromString(std::string::const_iterator begin, std::string::const_iterator end)
{ 
    CHAT_EXPECT(false, const_class_ERROR_MESSAGE);
    return begin;
}

template<typename CharT>
inline std::string::const_iterator ConstSerializer<std::basic_string<CharT>>::OnParseFromString(std::string::const_iterator begin, std::string::const_iterator end)
{ 
    CHAT_EXPECT(false, const_class_ERROR_MESSAGE);
    return begin;
}
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------



template class Serializer<std::basic_string<char>>;
template class Serializer<std::basic_string<wchar_t>>;
template class Serializer<std::basic_string<char16_t>>;
template class Serializer<std::basic_string<char32_t>>;

template class ConstSerializer<std::basic_string<char>>;
template class ConstSerializer<std::basic_string<wchar_t>>;
template class ConstSerializer<std::basic_string<char16_t>>;
template class ConstSerializer<std::basic_string<char32_t>>;

} /*END OF NAMESPACES*/



#endif // _NICKSV_CHAT_BSTRING_SERIALIZER_PASTE
