
#include <cstring>


#include "NickSV/Chat/Serializers.h"
#include "NickSV/Chat/ClientInfo.h"
#include "NickSV/Chat/Defines.h"


namespace NickSV::Chat {


template<typename CharT>
Serializer<ClientInfo<CharT>>::Serializer(ClientInfo<CharT>* p_CI) : m_pClientInfo(p_CI)
{ 
    CHAT_ASSERT(!m_pClientInfo, "m_pClientInfo must not be nullptr");
};


template<typename CharT>
inline size_t Serializer<ClientInfo<CharT>>::GetSize() const
{
    size_t size = 
      sizeof(api_version_type) +
      sizeof(m_pClientInfo->m_sNick.size()) +
      sizeof(char_type) * m_pClientInfo->m_sNick.size();
    return size + OnGetSize(size);
}

template<typename CharT>
 size_t Serializer<ClientInfo<CharT>>::OnGetSize(size_t baseSize) const { return 0; }


template<typename CharT>
std::string Serializer<ClientInfo<CharT>>::ToString() const
{
    std::string str;
    str.resize(GetSize());
    ToStringBuffer(str);
    return str;
}


template<typename CharT>
void Serializer<ClientInfo<CharT>>::ToStringBuffer(std::string& buffer) const
{
    CHAT_ASSERT(buffer.size() >= GetSize(), "Buffer size must be greater or equal than serialization size and preallocated by you");
    void* pvData = std::memcpy(buffer.data(), &m_pClientInfo->m_nAPIVer, sizeof(api_version_type));
    pvData = static_cast<char*>(pvData) + sizeof(api_version_type);
    size_t size = m_pClientInfo->m_sNick.size();
    std::memcpy(pvData, &size, sizeof(size_t));
    pvData = static_cast<char*>(pvData) + sizeof(size_t);
    std::memcpy(pvData, m_pClientInfo->m_sNick.data(), sizeof(char_type)*size);
    pvData = static_cast<char*>(pvData) + sizeof(char_type)*size;
    CHAT_ASSERT(pvData <= buffer.data() + GetSize(), "Something went wrong and the above code is broken");
    OnToString(pvData);
}


template<typename CharT>
ClientInfo<CharT> Serializer<ClientInfo<CharT>>::MakeFromString(const std::string& str)
{
    ClientInfo<CharT> clientInfo;
    if(clientInfo.GetSerializer().ParseFromString(str) != EResult::kSucces)
        clientInfo.SetInvalid();

    return clientInfo;
}


template<typename CharT>
EResult Serializer<ClientInfo<CharT>>::ParseFromString(const std::string& str) 
{
    if(sizeof(api_version_type) + sizeof(size_t) > str.size())
        return EResult::kError; //BAD INPUT. str size has to be at least sizeof(api_version_type) + sizeof(size_t) bytes long

    std::memcpy(&m_pClientInfo->m_nAPIVer, str.data(), sizeof(api_version_type));
    const void* pcvData = str.data() + sizeof(api_version_type);
    size_t nickSize;
    std::memcpy(&nickSize, pcvData, sizeof(size_t));
    pcvData = static_cast<const char*>(pcvData) + sizeof(size_t);
    if(static_cast<const char*>(pcvData) + sizeof(char_type)*nickSize > str.end().base())
        return EResult::kError; //BAD INPUT. Parsed size doesnt match with input size

    m_pClientInfo->m_sNick.resize(nickSize);
    std::memcpy(m_pClientInfo->m_sNick.data(), pcvData, sizeof(char_type)*nickSize);
    return OnParseFromString(pcvData);
}

template<typename CharT>
EResult inline Serializer<ClientInfo<CharT>>::OnParseFromString(const void* pcvNewPartStart) { return EResult::kSucces; }


template class Serializer<ClientInfo<char>>;
template class Serializer<ClientInfo<wchar_t>>;
template class Serializer<ClientInfo<char16_t>>;
template class Serializer<ClientInfo<char32_t>>;


} /*END OF NAMESPACES*/