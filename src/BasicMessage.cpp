#include <string>
#include <cstdint>


#include "NickSV/Chat/Utils.h"
#include "NickSV/Chat/Serializers/BMessageSerializer.h"
#include "NickSV/Chat/Parsers/BMessageParser.h"
#include "NickSV/Chat/BasicMessage.h"


namespace NickSV {
namespace Chat {


/*
MESSAGE.
Struct that user is sending to server
*/

template<typename CharT>
BasicMessage<CharT>::BasicMessage(BasicMessage<CharT>::UserIDType senderID) : m_nSenderID(senderID) {};

template<typename CharT>
BasicMessage<CharT>::BasicMessage(BasicMessage<CharT>::TextType rsText) : m_sText(std::move(rsText)) {};

template<typename CharT>
BasicMessage<CharT>::BasicMessage(BasicMessage<CharT>::UserIDType senderID, BasicMessage<CharT>::TextType rsText) 
    :  m_nSenderID(senderID), m_sText(std::move(rsText)) {};

template<typename CharT>
bool inline BasicMessage<CharT>::operator==(const BasicMessage<CharT>& other) const
{ 
    constexpr size_t size = sizeof(TextType) + sizeof(UserIDType) + sizeof(void*);
    Tools::type_integrity_assert_virtual<BasicMessage<CharT>, COMPILER_AWARE_VALUE(size,size,size)>();
    return (m_sText == other.m_sText) &&
           (m_nSenderID == other.m_nSenderID);
}

template<typename CharT>
bool inline BasicMessage<CharT>::operator!=(const BasicMessage<CharT>& other) const
{ return !operator==(other); }

template<typename CharT>
inline auto BasicMessage<CharT>::GetText() const
-> const typename BasicMessage<CharT>::TextType&
{ return m_sText; }

template<typename CharT>
inline auto BasicMessage<CharT>::GetText() 
-> typename BasicMessage<CharT>::TextType&
{ return m_sText; }

template<typename CharT>
auto BasicMessage<CharT>::GetSenderID()
-> typename BasicMessage<CharT>::UserIDType&
{ return m_nSenderID; }

template<typename CharT>
auto BasicMessage<CharT>::GetSenderID() const
-> typename BasicMessage<CharT>::UserIDType
{ return m_nSenderID; }

template<typename CharT>
auto BasicMessage<CharT>::GetSerializer() const
-> const std::unique_ptr<ISerializer>
{ 
    const auto ptr = new Serializer<BasicMessage<CharT>>(this);
    return std::unique_ptr<Serializer<BasicMessage<CharT>>>(ptr);
}


template class NICKSVCHAT_API BasicMessage<char>;
template class NICKSVCHAT_API BasicMessage<wchar_t>;
template class NICKSVCHAT_API BasicMessage<char16_t>;
template class NICKSVCHAT_API BasicMessage<char32_t>;


}}  /*END OF NAMESPACES*/