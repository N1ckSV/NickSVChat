
#ifndef _NICKSV_CHAT_MESSAGE_REQUEST_SERIALIZER_PASTE
#define _NICKSV_CHAT_MESSAGE_REQUEST_SERIALIZER_PASTE
#pragma once



#include "NickSV/Chat/Definitions.h"
#include "NickSV/Chat/Utils.h"
#include "NickSV/Chat/Serializers/MessageRequestSerializer.h"


namespace NickSV {
namespace Chat {






//----------------------------------------------------------------------------------------------------
// Serializer<MessageRequest> implementation
//----------------------------------------------------------------------------------------------------
Serializer<MessageRequest>::Serializer(const MessageRequest & rMessageRequest): m_rMessageRequest(rMessageRequest) {};

inline const MessageRequest& Serializer<MessageRequest>::GetObject() const
{ 
    return m_rMessageRequest;
};

inline size_t Serializer<MessageRequest>::GetSize() const
{
    Tools::type_integrity_assert<MessageRequest, COMPILER_AWARE_VALUE(16, 16, 16) + sizeof(std::unique_ptr<Message>)>();
    
    size_t size = 
      sizeof(ERequestType) +
      GetObject().GetMessage().GetSerializer()->GetSize();
    return size + OnGetSize(size);
}

inline size_t Serializer<MessageRequest>::OnGetSize(size_t) const { return 0; }

std::string Serializer<MessageRequest>::ToString() const
{
    std::string str;
    str.resize(GetSize());
    ToStringBuffer(str);
    return str;
}

std::string::iterator Serializer<MessageRequest>::ToString(std::string::iterator begin, std::string::iterator end) const
{
    CHAT_ASSERT(std::distance(begin, end) >= static_cast<ptrdiff_t>(GetSize()),
        invalid_range_size_ERROR_MESSAGE);

    Tools::type_integrity_assert<MessageRequest, COMPILER_AWARE_VALUE(16, 16, 16) + sizeof(std::unique_ptr<Message>)>();

    Transfer<ERequestType> type;
    type.Base = GetObject().GetType();
    std::string::iterator iter = std::copy(type.CharArr, type.CharArr + sizeof(ERequestType), begin);
    iter = GetObject().GetMessage().GetSerializer()->ToString(iter, end);
    CHAT_ASSERT(std::distance(iter, end) >= 0, something_went_wrong_ERROR_MESSAGE);
    return OnToString(iter, end);
}

inline std::string::iterator Serializer<MessageRequest>::OnToString(std::string::iterator begin, std::string::iterator) const
{
    return begin;
}

inline std::string::iterator Serializer<MessageRequest>::ToStringBuffer(std::string& buffer) const
{
    return ToString(buffer.begin(), buffer.end());
}
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------





}}  /*END OF NAMESPACES*/



#endif // _NICKSV_CHAT_MESSAGE_REQUEST_SERIALIZER_PASTE