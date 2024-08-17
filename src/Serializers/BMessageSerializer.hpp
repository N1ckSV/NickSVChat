
#ifndef _NICKSV_CHAT_BMESSAGE_SERIALIZER_PASTE
#define _NICKSV_CHAT_BMESSAGE_SERIALIZER_PASTE
#pragma once



#include "NickSV/Chat/Serializers/BStringSerializer.h"
#include "NickSV/Chat/Serializers/BMessageSerializer.h"
#include "NickSV/Chat/BasicMessage.h"


namespace NickSV {
namespace Chat {






//----------------------------------------------------------------------------------------------------
// Serializer<BasicMessage<CharT>> implementation
//----------------------------------------------------------------------------------------------------
template<typename CharT>
Serializer<BasicMessage<CharT>>::Serializer(const BasicMessage<CharT> * const  pBasicMessage) : m_pBasicMessage(pBasicMessage)
{ 
    CHAT_ASSERT(m_pBasicMessage, "m_pBasicMessage must not be nullptr");
};

template<typename CharT>
inline const BasicMessage<CharT>* Serializer<BasicMessage<CharT>>::GetObject() const
{ 
    return m_pBasicMessage;
};

template<typename CharT>
inline size_t Serializer<BasicMessage<CharT>>::GetSize() const
{
    using UserIDType = typename BasicMessage<CharT>::UserIDType;
    using TextType = typename BasicMessage<CharT>::TextType;
    using CharType = typename BasicMessage<CharT>::CharType;

    Tools::type_integrity_assert<BasicMessage<CharT>, 
        COMPILER_AWARE_VALUE(8, 8, 8) +
        sizeof(UserIDType) + 
        sizeof(TextType)>();

    size_t size = 
      sizeof(UserIDType) +
      sizeof(GetObject()->GetText().size()) +
      sizeof(CharType) * GetObject()->GetText().size();
    return size + OnGetSize(size);
}

template<typename CharT>
inline size_t Serializer<BasicMessage<CharT>>::OnGetSize(size_t) const { return 0; }

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
    using UserIDType = typename BasicMessage<CharT>::UserIDType;
    using TextType = typename BasicMessage<CharT>::TextType;

    CHAT_ASSERT(end >= GetSize() + begin, invalid_range_size_ERROR_MESSAGE);
    Tools::type_integrity_assert<BasicMessage<CharT>, 
        COMPILER_AWARE_VALUE(8, 8, 8) +
        sizeof(UserIDType) + 
        sizeof(TextType)>();

    Transfer<UserIDType> id;
    id.Base = GetObject()->GetSenderID();
    auto iter = std::copy(id.CharArr, id.CharArr + sizeof(UserIDType), begin);
    iter = Serializer<std::basic_string<CharT>>(&GetObject()->GetText()).ToString(iter, end);
    CHAT_ASSERT(iter <= end, something_went_wrong_ERROR_MESSAGE);
    return OnToString(iter, end);
}

template<typename CharT>
inline std::string::iterator Serializer<BasicMessage<CharT>>::OnToString(std::string::iterator begin, std::string::iterator) const
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




}}  /*END OF NAMESPACES*/



#endif // _NICKSV_CHAT_BMESSAGE_SERIALIZER_PASTE