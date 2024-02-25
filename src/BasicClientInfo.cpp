
#include <string>
#include <cstdint>


#include "NickSV/Chat/BasicClientInfo.h"
#include "NickSV/Chat/Serializers/BClientInfoSerializer.h"
#include "NickSV/Chat/Utils.h"


namespace NickSV::Chat {


/*
API CLIENT INFO.
App's additional information should be inheritated from this struct
*/


template<typename CharT>
BasicClientInfo<CharT>::BasicClientInfo(const NickType& rsNick) : m_sNick(rsNick) {};

template<typename CharT>
BasicClientInfo<CharT>::BasicClientInfo(NickType&& rsNick) noexcept : m_sNick(std::move(rsNick)) {};

template<typename CharT>
BasicClientInfo<CharT>::BasicClientInfo(BasicClientInfo&& rhs) noexcept : m_nAPIVer(rhs.m_nAPIVer) { m_sNick = std::move(rhs.m_sNick); };

template<typename CharT>
inline void BasicClientInfo<CharT>::SetInvalid()
{
    m_nAPIVer = 0;
    m_sNick = basic_string_cast<CharT>("Invalid");
}

template<typename CharT>
const BasicClientInfo<CharT>& BasicClientInfo<CharT>::operator=(const BasicClientInfo<CharT>& rhs)
{
    if (this == std::addressof(rhs)) [[unlikely]] return *this;
    m_nAPIVer = rhs.m_nAPIVer;
    m_sNick = rhs.m_sNick;
    return *this;
};

template<typename CharT>
BasicClientInfo<CharT>& BasicClientInfo<CharT>::operator=(BasicClientInfo<CharT>&& rhs) noexcept
{
    if (this == std::addressof(rhs)) [[unlikely]] return *this;
    m_nAPIVer = rhs.m_nAPIVer;
    std::swap(m_sNick, rhs.m_sNick);
    return *this;
};

template<typename CharT>
bool BasicClientInfo<CharT>::IsValid() const { return GetAPIVer() > 0; }

template<typename CharT>
typename BasicClientInfo<CharT>::APIVersionType BasicClientInfo<CharT>::GetAPIVer() const { return m_nAPIVer; }

template<typename CharT>
bool inline BasicClientInfo<CharT>::operator==(const BasicClientInfo<CharT>& other) const
{ 
    return m_nAPIVer == other.m_nAPIVer &&
           m_sNick == other.m_sNick;
}

template<typename CharT>
bool inline BasicClientInfo<CharT>::operator!=(const BasicClientInfo<CharT>& other) const { return !operator==(other); }

template<typename CharT>
inline typename BasicClientInfo<CharT>::NickType& BasicClientInfo<CharT>::GetNickname() { return m_sNick; }

template<typename CharT>
const inline typename BasicClientInfo<CharT>::NickType& BasicClientInfo<CharT>::GetNickname() const { return m_sNick; }

template class BasicClientInfo<char>;
template class BasicClientInfo<wchar_t>;
template class BasicClientInfo<char16_t>;
template class BasicClientInfo<char32_t>;


} /*END OF NAMESPACES*/