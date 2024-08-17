
#ifndef _NICKSV_CHAT_REQUEST_PARSER_PASTE
#define _NICKSV_CHAT_REQUEST_PARSER_PASTE
#pragma once



#include "NickSV/Chat/Types.h"
#include "NickSV/Chat/Utils.h"

#include "NickSV/Chat/Requests/Request.h"
#include "NickSV/Chat/Requests/MessageRequest.h"
#include "NickSV/Chat/Requests/ClientInfoRequest.h"

#include "NickSV/Chat/Parsers/RequestParser.h"
#include "NickSV/Chat/Parsers/ClientInfoRequestParser.h"
#include "NickSV/Chat/Parsers/MessageRequestParser.h"


namespace NickSV {
namespace Chat {




//----------------------------------------------------------------------------------------------------
// Parser<Request> implementation
//----------------------------------------------------------------------------------------------------

Parser<Request>::Parser() : m_upRequest(new Request()) {};

Request& Parser<Request>::GetObject()
{ 
    return *m_upRequest;
};

inline std::string::const_iterator Parser<Request>::FromString(const std::string& str)
{
    return FromString(str.cbegin(), str.cend());
}

std::string::const_iterator Parser<Request>::FromString(std::string::const_iterator begin, std::string::const_iterator end)
{
    if(std::distance(begin, end) < static_cast<std::ptrdiff_t>(sizeof(ERequestType)))
        return begin; //BAD INPUT. Range size has to be at least sizeof(ERequestType) bytes long

    ERequestType type;
    auto iter = ParseSeries(begin, end, type);
    if(std::distance(begin, iter) <= 0)
        return begin;

    if(type == ERequestType::ClientInfo)
    {
        m_upRequest.reset(new ClientInfoRequest());
        iter = ParseSeries(begin, end, static_cast<ClientInfoRequest&>(GetObject()));
    }
    else if(type == ERequestType::Message)
    {
        m_upRequest.reset(new MessageRequest());
        iter = ParseSeries(begin, end, static_cast<MessageRequest&>(GetObject()));
    }
    else
        iter = OnFromString(begin, end);

    if(std::distance(begin, iter) <= 0)
        return begin;

    auto newIter = OnFromString(iter, end);
    if(iter == end)
        return end;
    if(std::distance(iter, newIter) <= 0)
        return begin;
    return newIter;
}

std::string::const_iterator inline Parser<Request>::OnFromString(
    std::string::const_iterator,
    std::string::const_iterator end)
{ 
    return end;
}
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------




}}  /*END OF NAMESPACES*/



#endif // _NICKSV_CHAT_REQUEST_PARSER_PASTE