#include <string>
#include <cstdint>


#include "NickSV/Chat/BasicMessage.h"
#include "NickSV/Chat/Serializers/BMessageSerializer.h"
#include "NickSV/Chat/Utils.h"


namespace NickSV::Chat {


/*
API MESSAGE.
Struct that user is sending to server
*/


template<typename CharT>
BasicMessage<CharT>::BasicMessage(const text_type& rsText) : m_sText(rsText) {};

template<typename CharT>
BasicMessage<CharT>::BasicMessage(text_type&& rsText) noexcept : m_sText(std::move(rsText)) {};

template<typename CharT>
BasicMessage<CharT>::BasicMessage(BasicMessage&& other) noexcept : m_sText(std::move(other.m_sText)) {};

template<typename CharT>
const BasicMessage<CharT>& BasicMessage<CharT>::operator=(const BasicMessage<CharT>& rhs)
{
    if (this == std::addressof(rhs)) [[unlikely]] return *this;
    m_sText = rhs.m_sText;
    return *this;
};

template<typename CharT>
BasicMessage<CharT>& BasicMessage<CharT>::operator=(BasicMessage<CharT>&& rhs) noexcept
{
    if (this == std::addressof(rhs)) [[unlikely]] return *this;
    std::swap(m_sText, rhs.m_sText);
    return *this;
};


template<typename CharT>
bool inline BasicMessage<CharT>::operator==(const BasicMessage<CharT>& other) const
{ 
    return (m_sText == other.m_sText);
}

template<typename CharT>
bool inline BasicMessage<CharT>::operator!=(const BasicMessage<CharT>& other) const { return !operator==(other); }

template<typename CharT>
const inline typename BasicMessage<CharT>::text_type& BasicMessage<CharT>::GetText() const { return m_sText; }

template<typename CharT>
inline typename BasicMessage<CharT>::text_type& BasicMessage<CharT>::GetText() { return m_sText; }

template class BasicMessage<char>;
template class BasicMessage<wchar_t>;
template class BasicMessage<char16_t>;
template class BasicMessage<char32_t>;


} /*END OF NAMESPACES*/