
#ifndef _NICKSV_CHAT_MESSAGE_REQUEST_PARSER_PASTE
#define _NICKSV_CHAT_MESSAGE_REQUEST_PARSER_PASTE
#pragma once



#include "NickSV/Chat/Types.h"
#include "NickSV/Chat/Utils.h"

#include "NickSV/Chat/Parsers/MessageRequestParser.h"

#include "NickSV/Chat/Requests/MessageRequest.h"

#include "NickSV/Chat/BasicMessage.h"

namespace NickSV {
namespace Chat {




//----------------------------------------------------------------------------------------------------
// Parser<MessageRequest> implementation
//----------------------------------------------------------------------------------------------------
Parser<MessageRequest>::Parser() : m_upMessageRequest(new MessageRequest()) {};

MessageRequest& Parser<MessageRequest>::GetObject()
{ 
    return *m_upMessageRequest;
};

inline std::string::const_iterator Parser<MessageRequest>::FromString(const std::string& str)
{
    return FromString(str.cbegin(), str.cend());
}

std::string::const_iterator Parser<MessageRequest>::FromString(std::string::const_iterator begin, std::string::const_iterator end)
{
    Tools::type_integrity_assert<MessageRequest, COMPILER_AWARE_VALUE(16, 16, 16) + sizeof(std::unique_ptr<Message>)>();
    if(std::distance(begin, end) < static_cast<ptrdiff_t>(sizeof(ERequestType)))
        return begin; //BAD INPUT. Range size has to be at least sizeof(ERequestType) bytes long

    ERequestType type;
    auto iter = ParseSeries(begin, end, type);
    if(std::distance(begin, iter) <= 0 ||
       type != ERequestType::Message)
            return begin;

    auto newIter = ParseSeries(iter, end, GetObject().GetMessage());
    if(std::distance(iter, newIter) <= 0)
        return begin;

    iter = OnFromString(newIter, end);
    if(newIter == end)
        return end;
    if(std::distance(newIter, iter) <= 0)
        return begin;
    return iter;
}

std::string::const_iterator inline Parser<MessageRequest>::OnFromString(
    std::string::const_iterator,
    std::string::const_iterator end)
{ 
    return end; 
}
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------



}}  /*END OF NAMESPACES*/



#endif // _NICKSV_CHAT_MESSAGE_REQUEST_PARSER_PASTE