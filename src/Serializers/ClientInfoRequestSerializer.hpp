
#ifndef _NICKSV_CHAT_CLIENTINFO_REQUEST_SERIALIZER_PASTE
#define _NICKSV_CHAT_CLIENTINFO_REQUEST_SERIALIZER_PASTE
#pragma once


#include "NickSV/Chat/Definitions.h"
#include "NickSV/Chat/Utils.h"
#include "NickSV/Chat/Serializers/ClientInfoRequestSerializer.h"


namespace NickSV {
namespace Chat {






//----------------------------------------------------------------------------------------------------
// Serializer<ClientInfoRequest> implementation
//----------------------------------------------------------------------------------------------------
Serializer<ClientInfoRequest>::Serializer(const ClientInfoRequest* const cpcClientInfoRequest) : m_cpcClientInfoRequest(cpcClientInfoRequest)
{ 
    CHAT_ASSERT(m_cpcClientInfoRequest, "Serializer constructor parameter must not be nullptr");
};

inline const ClientInfoRequest* Serializer<ClientInfoRequest>::GetObject() const
{ 
    return m_cpcClientInfoRequest;
};

inline size_t Serializer<ClientInfoRequest>::GetSize() const
{
    size_t size = 
      sizeof(ERequestType) +
      GetObject()->GetClientInfo()->GetSerializer()->GetSize();
    return size + OnGetSize(size);
}

inline size_t Serializer<ClientInfoRequest>::OnGetSize(size_t) const { return 0; }

std::string Serializer<ClientInfoRequest>::ToString() const
{
    std::string str;
    str.resize(GetSize());
    ToStringBuffer(str);
    return str;
}

std::string::iterator Serializer<ClientInfoRequest>::ToString(std::string::iterator begin, std::string::iterator end) const
{
    CHAT_ASSERT(end >= begin + GetSize(), invalid_range_size_ERROR_MESSAGE);
    Transfer<ERequestType> type;
    type.Base = GetObject()->GetType();
    std::string::iterator iter = std::copy(type.CharArr, type.CharArr + sizeof(ERequestType), begin);
    iter = GetObject()->GetClientInfo()->GetSerializer()->ToString(iter, end);
    CHAT_ASSERT(iter <= end, something_went_wrong_ERROR_MESSAGE);
    return OnToString(iter, end);
}

inline std::string::iterator Serializer<ClientInfoRequest>::OnToString(std::string::iterator begin, std::string::iterator) const
{
    return begin;
}

inline std::string::iterator Serializer<ClientInfoRequest>::ToStringBuffer(std::string& buffer) const
{
    return ToString(buffer.begin(), buffer.end());
}
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------


template class Serializer<ClientInfoRequest>;



}}  /*END OF NAMESPACES*/



#endif // _NICKSV_CHAT_CLIENTINFO_REQUEST_SERIALIZER_PASTE