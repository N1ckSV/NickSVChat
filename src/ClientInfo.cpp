
#include <string>
#include <cstdint>

#include "NickSV/Chat/ClientInfo.h"
#include "NickSV/Chat/Serializers.h"
#include "NickSV/Chat/Utils.h"


namespace NickSV::Chat {


/*
API CLIENT INFO.
App's additional information should be inheritated from this struct
*/


template<class CharT>
ClientInfo<CharT>::ClientInfo(ClientInfo&& rhs) noexcept : m_nAPIVer(rhs.m_nAPIVer) { m_sNick = std::move(rhs.m_sNick); };

template<class CharT>
inline void ClientInfo<CharT>::SetInvalid()
{
    m_nAPIVer = 0;
    m_sNick = basic_string_cast<CharT>("Invalid");
}

template<class CharT>
const ClientInfo<CharT>& ClientInfo<CharT>::operator=(const ClientInfo<CharT>& rhs)
{
    if (this == std::addressof(rhs)) [[unlikely]] return *this;
    m_nAPIVer = rhs.m_nAPIVer;
    m_sNick = rhs.m_sNick;
    return *this;
};

template<class CharT>
ClientInfo<CharT>& ClientInfo<CharT>::operator=(ClientInfo<CharT>&& rhs) noexcept
{
    if (this == std::addressof(rhs)) [[unlikely]] return *this;
    m_nAPIVer = rhs.m_nAPIVer;
    std::swap(m_sNick, rhs.m_sNick);
    return *this;
};

template<class CharT>
bool ClientInfo<CharT>::IsValid() const { return GetAPIVer() > 0; }

template<class CharT>
ClientInfoSerializer<CharT> ClientInfo<CharT>::GetSerializer() { return ClientInfoSerializer<CharT>(this); }

template<class CharT>
typename ClientInfo<CharT>::api_version_type ClientInfo<CharT>::GetAPIVer() const { return m_nAPIVer; }

template<class CharT>
bool inline ClientInfo<CharT>::operator==(const ClientInfo<char_type>& other) const
{ 
    return m_nAPIVer == other.m_nAPIVer &&
           m_sNick == other.m_sNick;
}

template<class CharT>
bool inline ClientInfo<CharT>::operator!=(const ClientInfo<char_type>& other) const { return !operator==(other); }

template class ClientInfo<char>;
template class ClientInfo<wchar_t>;
template class ClientInfo<char16_t>;
template class ClientInfo<char32_t>;


} /*END OF NAMESPACES*/