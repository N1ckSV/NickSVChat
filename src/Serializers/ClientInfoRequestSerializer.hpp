
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
Serializer<ClientInfoRequest>::Serializer(const ClientInfoRequest& rClientInfoRequest) : m_rClientInfoRequest(rClientInfoRequest) {};

inline const ClientInfoRequest& Serializer<ClientInfoRequest>::GetObject() const
{ 
    return m_rClientInfoRequest;
};

inline size_t Serializer<ClientInfoRequest>::GetSize() const
{
    Tools::type_integrity_assert<ClientInfoRequest, COMPILER_AWARE_VALUE(16, 16, 16) + sizeof(std::unique_ptr<ClientInfo>)>();
    
    size_t size = 
      sizeof(ERequestType) +
      GetObject().GetClientInfo().GetSerializer()->GetSize();
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
    CHAT_ASSERT(std::distance(begin, end) >= static_cast<ptrdiff_t>(GetSize()),
        invalid_range_size_ERROR_MESSAGE);

    Transfer<ERequestType> type;
    type.Base = GetObject().GetType();
    std::string::iterator iter = std::copy(type.CharArr, type.CharArr + sizeof(ERequestType), begin);
    iter = GetObject().GetClientInfo().GetSerializer()->ToString(iter, end);
    CHAT_ASSERT(std::distance(iter, end) >= 0, something_went_wrong_ERROR_MESSAGE);
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





}}  /*END OF NAMESPACES*/



#endif // _NICKSV_CHAT_CLIENTINFO_REQUEST_SERIALIZER_PASTE