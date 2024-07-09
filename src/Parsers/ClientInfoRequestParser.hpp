
#ifndef _NICKSV_CHAT_CLIENTINFO_REQUEST_PARSER_PASTE
#define _NICKSV_CHAT_CLIENTINFO_REQUEST_PARSER_PASTE
#pragma once



#include "NickSV/Chat/Types.h"
#include "NickSV/Chat/ClientInfo.h"

#include "NickSV/Chat/Parsers/ClientInfoRequestParser.h"
#include "NickSV/Chat/Requests/ClientInfoRequest.h"

#include "NickSV/Chat/Utils.h"


namespace NickSV {
namespace Chat {






//----------------------------------------------------------------------------------------------------
// Parser<ClientInfoRequest> implementation
//----------------------------------------------------------------------------------------------------
Parser<ClientInfoRequest>::Parser() : m_upClientInfoRequest(std::make_unique<ClientInfoRequest>()) {};

std::unique_ptr<ClientInfoRequest>& Parser<ClientInfoRequest>::GetObject()
{ 
    return m_upClientInfoRequest;
};

std::unique_ptr<Parser<ClientInfo>> Parser<ClientInfoRequest>::GetClientInfoParser()
{
    return std::make_unique<Parser<ClientInfo>>();
}

inline std::string::const_iterator Parser<ClientInfoRequest>::FromString(const std::string& str)
{
    return FromString(str.cbegin(), str.cend());
}

std::string::const_iterator Parser<ClientInfoRequest>::FromString(std::string::const_iterator begin, std::string::const_iterator end)
{
    Tools::type_integrity_assert<ClientInfoRequest, COMPILER_AWARE_VALUE(16, 16, 16) + sizeof(std::unique_ptr<ClientInfo>)>();
    if(begin + sizeof(ERequestType) > end)
        return begin; //BAD INPUT. Range size has to be at least sizeof(ERequestType) bytes long

    Transfer<ERequestType> type;
    std::copy(begin, begin + sizeof(ERequestType), type.CharArr);
    if(type.Base != ERequestType::ClientInfo)
        return begin;

    auto iter = begin + sizeof(ERequestType);
    auto parser = GetClientInfoParser();
    auto newIter = parser->FromString(iter, end);
    if(newIter <= iter)
        return begin;

    std::swap(GetObject()->GetClientInfo(), parser->GetObject());
    return OnFromString(iter, end);
}

std::string::const_iterator inline Parser<ClientInfoRequest>::OnFromString(
    std::string::const_iterator begin,
    std::string::const_iterator)
{ 
    return begin; 
}
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------


template class Parser<ClientInfoRequest>;


}}  /*END OF NAMESPACES*/



#endif // _NICKSV_CHAT_CLIENTINFO_REQUEST_PARSER_PASTE