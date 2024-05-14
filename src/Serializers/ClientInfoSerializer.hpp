
#ifndef _NICKSV_CHAT_CLIENTINFO_SERIALIZER_PASTE
#define _NICKSV_CHAT_CLIENTINFO_SERIALIZER_PASTE
#pragma once


#include "NickSV/Chat/Definitions.h"
#include "NickSV/Chat/Utils.h"
#include "NickSV/Chat/Serializers/BStringSerializer.h"
#include "NickSV/Chat/Serializers/ClientInfoSerializer.h"


namespace NickSV::Chat {






//----------------------------------------------------------------------------------------------------
// Serializer<ClientInfo> implementation
//----------------------------------------------------------------------------------------------------

Serializer<ClientInfo>::Serializer(const ClientInfo* const cpcClientInfo) : m_cpcClientInfo(cpcClientInfo)
{ 
    CHAT_ASSERT(m_cpcClientInfo, "Serializer constructor parameter must not be nullptr");
};

inline const ClientInfo* const Serializer<ClientInfo>::GetObject() const
{ 
    return m_cpcClientInfo;
};

inline size_t Serializer<ClientInfo>::GetSize() const
{
    size_t size = 
      sizeof(LibVersionType) +
      sizeof(EState) +
      sizeof(UserIDType); 
    return size + OnGetSize(size);
}

inline size_t Serializer<ClientInfo>::OnGetSize(size_t baseSize) const { return 0; }

std::string Serializer<ClientInfo>::ToString() const
{
    std::string str;
    str.resize(GetSize());
    ToStringBuffer(str);
    return str;
}

std::string::iterator Serializer<ClientInfo>::ToString(std::string::iterator begin, std::string::iterator end) const
{
    CHAT_ASSERT(end >= begin + GetSize(), invalid_range_size_ERROR_MESSAGE);
    Transfer<LibVersionType> ver;
    ver.Base = GetObject()->GetLibVer();
    auto iter = std::copy(ver.CharArr, ver.CharArr + sizeof(LibVersionType), begin);
    Transfer<EState> state;
    state.Base = GetObject()->GetState();
    iter = std::copy(state.CharArr, state.CharArr + sizeof(EState), iter);
    Transfer<UserIDType> id;
    id.Base = GetObject()->GetUserID();
    iter = std::copy(id.CharArr, id.CharArr + sizeof(UserIDType), iter);
    CHAT_ASSERT(iter <= end, something_went_wrong_ERROR_MESSAGE);
    return OnToString(iter, end);
}

inline std::string::iterator Serializer<ClientInfo>::OnToString(std::string::iterator begin, std::string::iterator end) const
{
    return begin;
}

inline std::string::iterator Serializer<ClientInfo>::ToStringBuffer(std::string& buffer) const
{
    return ToString(buffer.begin(), buffer.end());
}
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------


template class Serializer<ClientInfo>;


} /*END OF NAMESPACES*/



#endif // _NICKSV_CHAT_CLIENTINFO_SERIALIZER_PASTE