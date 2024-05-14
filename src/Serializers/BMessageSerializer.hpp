
#ifndef _NICKSV_CHAT_BMESSAGE_SERIALIZER_PASTE
#define _NICKSV_CHAT_BMESSAGE_SERIALIZER_PASTE
#pragma once



#include "NickSV/Chat/Definitions.h"
#include "NickSV/Chat/Utils.h"
#include "NickSV/Chat/BasicMessage.h"
#include "NickSV/Chat/Serializers/BStringSerializer.h"
#include "NickSV/Chat/Serializers/BMessageSerializer.h"


namespace NickSV::Chat {






//----------------------------------------------------------------------------------------------------
// Serializer<BasicMessage<CharT>> implementation
//----------------------------------------------------------------------------------------------------
template<typename CharT>
Serializer<BasicMessage<CharT>>::Serializer(const BasicMessage<CharT> * const  cpcBasicMessage) : m_cpcBasicMessage(cpcBasicMessage)
{ 
    CHAT_ASSERT(m_cpcBasicMessage, "m_cpBasicMessage must not be nullptr");
};

template<typename CharT>
inline const BasicMessage<CharT>* const Serializer<BasicMessage<CharT>>::GetObject() const
{ 
    return m_cpcBasicMessage;
};

template<typename CharT>
inline size_t Serializer<BasicMessage<CharT>>::GetSize() const
{
    size_t size = 
      sizeof(GetObject()->GetText().size()) +
      sizeof(CharType) * GetObject()->GetText().size();
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
    auto iter = Serializer<std::basic_string<CharT>>(&GetObject()->GetText()).ToString(begin, end);
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
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------



template class Serializer<BasicMessage<char>>;
template class Serializer<BasicMessage<wchar_t>>;
template class Serializer<BasicMessage<char16_t>>;
template class Serializer<BasicMessage<char32_t>>;


} /*END OF NAMESPACES*/



#endif // _NICKSV_CHAT_BMESSAGE_SERIALIZER_PASTE