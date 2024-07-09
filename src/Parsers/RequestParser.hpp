
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

Parser<Request>::Parser() : m_upRequest(std::make_unique<Request>()) {};

std::unique_ptr<Request>& Parser<Request>::GetObject()
{ 
    return m_upRequest;
};

std::unique_ptr<Parser<ClientInfoRequest>> Parser<Request>::GetClientInfoRequestParser()
{
    return std::make_unique<Parser<ClientInfoRequest>>();
}

std::unique_ptr<Parser<MessageRequest>>    Parser<Request>::GetMessageRequestParser()
{
    return std::make_unique<Parser<MessageRequest>>();
}

inline std::string::const_iterator Parser<Request>::FromString(const std::string& str)
{
    return FromString(str.cbegin(), str.cend());
}

std::string::const_iterator Parser<Request>::FromString(std::string::const_iterator begin, std::string::const_iterator end)
{
    if(begin + sizeof(ERequestType) > end)
        return begin; //BAD INPUT. Range size has to be at least sizeof(ERequestType) bytes long

    Transfer<ERequestType> type;
    std::copy(begin, begin + sizeof(ERequestType), type.CharArr);
    auto iter = begin;
    switch (type.Base)
    {
        case ERequestType::ClientInfo:
        {
            auto parser = GetClientInfoRequestParser();
            iter = parser->FromString(begin, end);
            if(iter == begin)
                return begin;

            std::unique_ptr<Request> request(std::move(parser->GetObject()));
            std::swap(GetObject(), request);
            break;
        }
        case ERequestType::Message:
        {
            auto parser = GetMessageRequestParser();
            iter = parser->FromString(begin, end);
            if(iter == begin)
                return begin;

            std::unique_ptr<Request> request(std::move(parser->GetObject()));
            std::swap(GetObject(), request);
            break;
        }
        default:
            break;
    }
    if(iter == begin)
        return OnFromString(begin, end);
        
    return OnFromString(iter, end);
}

std::string::const_iterator inline Parser<Request>::OnFromString(
    std::string::const_iterator begin,
    std::string::const_iterator)
{ 
    return begin;
}
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------



template class Parser<Request>;


}}  /*END OF NAMESPACES*/



#endif // _NICKSV_CHAT_REQUEST_PARSER_PASTE