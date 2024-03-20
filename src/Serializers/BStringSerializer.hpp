
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
Serializer<std::basic_string<CharT>>::Serializer(const std::basic_string<CharT>* const cpcBasicString) : m_cpcBasicString(cpcBasicString)
{ 
    CHAT_ASSERT(m_cpcBasicString, "m_cpcBasicString must not be nullptr");
};

template<typename CharT>
inline const std::basic_string<CharT>* const Serializer<std::basic_string<CharT>>::GetObject() const
{ 
    return m_cpcBasicString;
};

template<typename CharT>
inline size_t Serializer<std::basic_string<CharT>>::GetSize() const
{
    size_t size =
      sizeof(size_t) +
      sizeof(CharType) * GetObject()->size();
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
    CHAT_ASSERT(end >= begin + GetSize(), invalid_range_size_ERROR_MESSAGE);
    Transfer<size_t> stringSize;
    stringSize.Base = GetObject()->size();
    auto iter = std::copy(stringSize.CharArr, stringSize.CharArr + sizeof(size_t), begin);
    iter = std::copy(reinterpret_cast<const char*>(GetObject()->cbegin().base()),
                     reinterpret_cast<const char*>(GetObject()->cend().base()), iter);
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
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------




template class Serializer<std::basic_string<char>>;
template class Serializer<std::basic_string<wchar_t>>;
template class Serializer<std::basic_string<char16_t>>;
template class Serializer<std::basic_string<char32_t>>;

} /*END OF NAMESPACES*/



#endif // _NICKSV_CHAT_BSTRING_SERIALIZER_PASTE
