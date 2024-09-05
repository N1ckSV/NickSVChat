
#ifndef _NICKSV_CHAT_MESSAGE_SERIALIZER_PASTE
#define _NICKSV_CHAT_MESSAGE_SERIALIZER_PASTE
#pragma once



#include "NickSV/Chat/Serializers/BStringSerializer.h"
#include "NickSV/Chat/Serializers/MessageSerializer.h"


namespace NickSV {
namespace Chat {






//----------------------------------------------------------------------------------------------------
// Serializer<Message> implementation
//----------------------------------------------------------------------------------------------------
Serializer<Message>::Serializer(const Message &  rMessage) : m_rMessage(rMessage) {};
inline const Message& Serializer<Message>::GetObject() const
{ 
    return m_rMessage;
};

inline size_t Serializer<Message>::GetSize() const
{
    using UserIDType = typename Message::UserIDType;
    using TextType = typename Message::TextType;
    using CharType = typename Message::CharType;

    Tools::type_integrity_assert<Message, 
        COMPILER_AWARE_VALUE(8, 8, 8) +
        sizeof(UserIDType) + 
        sizeof(TextType)>();

    size_t size = 
      sizeof(UserIDType) +
      sizeof(GetObject().GetText().size()) +
      sizeof(CharType) * GetObject().GetText().size();
    return size + OnGetSize(size);
}

inline size_t Serializer<Message>::OnGetSize(size_t) const { return 0; }

std::string Serializer<Message>::ToString() const
{
    std::string str;
    str.resize(GetSize());
    ToStringBuffer(str);
    return str;
}

std::string::iterator Serializer<Message>::ToString(std::string::iterator begin, std::string::iterator end) const
{
    using UserIDType = typename Message::UserIDType;
    using TextType = typename Message::TextType;

    CHAT_ASSERT(std::distance(begin, end) >= static_cast<ptrdiff_t>(GetSize()),
        invalid_range_size_ERROR_MESSAGE);
    Tools::type_integrity_assert<Message, 
        COMPILER_AWARE_VALUE(8, 8, 8) +
        sizeof(UserIDType) + 
        sizeof(TextType)>();

    Transfer<UserIDType> id;
    id.Base = GetObject().GetSenderID();
    auto iter = std::copy(id.CharArr, id.CharArr + sizeof(UserIDType), begin);
    iter = Serializer<TextType>(GetObject().GetText()).ToString(iter, end);
    CHAT_ASSERT(std::distance(iter, end) >= 0, something_went_wrong_ERROR_MESSAGE);
    return OnToString(iter, end);
}

inline std::string::iterator Serializer<Message>::OnToString(std::string::iterator begin, std::string::iterator) const
{
    return begin;
}

inline std::string::iterator Serializer<Message>::ToStringBuffer(std::string& buffer) const
{
    return ToString(buffer.begin(), buffer.end());
}
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------




}}  /*END OF NAMESPACES*/



#endif // _NICKSV_CHAT_MESSAGE_SERIALIZER_PASTE