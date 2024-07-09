
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
Parser<MessageRequest>::Parser() : m_upMessageRequest(std::make_unique<MessageRequest>()) {};

std::unique_ptr<MessageRequest>& Parser<MessageRequest>::GetObject()
{ 
    return m_upMessageRequest;
};

std::unique_ptr<Parser<Message>> Parser<MessageRequest>::GetMessageParser()
{
    return std::make_unique<Parser<Message>>();
}

inline std::string::const_iterator Parser<MessageRequest>::FromString(const std::string& str)
{
    return FromString(str.cbegin(), str.cend());
}

std::string::const_iterator Parser<MessageRequest>::FromString(std::string::const_iterator begin, std::string::const_iterator end)
{
    Tools::type_integrity_assert<MessageRequest, COMPILER_AWARE_VALUE(16, 16, 16) + sizeof(std::unique_ptr<Message>)>();
    if(begin + sizeof(ERequestType) > end)
        return begin; //BAD INPUT. Range size has to be at least sizeof(ERequestType) bytes long

    Transfer<ERequestType> type;
    std::copy(begin, begin + sizeof(ERequestType), type.CharArr);
    if(type.Base != ERequestType::Message)
        return begin;

    auto iter = begin + sizeof(ERequestType);
    auto parser = GetMessageParser();
    auto newIter = parser->FromString(iter, end);
    if(newIter <= iter)
        return begin;

    std::swap(GetObject()->GetMessage(), parser->GetObject());
    return OnFromString(iter, end);
}

std::string::const_iterator inline Parser<MessageRequest>::OnFromString(
    std::string::const_iterator begin,
    std::string::const_iterator)
{ 
    return begin; 
}
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------


template class Parser<MessageRequest>;


}}  /*END OF NAMESPACES*/



#endif // _NICKSV_CHAT_MESSAGE_REQUEST_PARSER_PASTE