#include <string>
#include <cstdint>


#include "NickSV/Chat/BasicMessage.h"
#include "NickSV/Chat/Serializers/BMessageSerializer.h"
#include "NickSV/Chat/Utils.h"


namespace NickSV::Chat {


/*
MESSAGE.
Struct that user is sending to server
*/


template<typename CharT>
BasicMessage<CharT>::BasicMessage(const BasicMessage<CharT>::TextType& rsText) : m_sText(rsText) {};

template<typename CharT>
BasicMessage<CharT>::BasicMessage(BasicMessage<CharT>::TextType&& rsText) noexcept : m_sText(std::move(rsText)) {};


template<typename CharT>
bool inline BasicMessage<CharT>::operator==(const BasicMessage<CharT>& other) const
{ 
    return (m_sText == other.m_sText);
}

template<typename CharT>
bool inline BasicMessage<CharT>::operator!=(const BasicMessage<CharT>& other) const { return !operator==(other); }

template<typename CharT>
const inline typename BasicMessage<CharT>::TextType& BasicMessage<CharT>::GetText() const { return m_sText; }

template<typename CharT>
inline typename BasicMessage<CharT>::TextType& BasicMessage<CharT>::GetText() { return m_sText; }

template<typename CharT>
const std::unique_ptr<ISerializer> BasicMessage<CharT>::GetSerializer() const 
{ 
    return std::make_unique<Serializer<BasicMessage<CharT>>>(this); 
}

template class BasicMessage<char>;
template class BasicMessage<wchar_t>;
template class BasicMessage<char16_t>;
template class BasicMessage<char32_t>;


} /*END OF NAMESPACES*/