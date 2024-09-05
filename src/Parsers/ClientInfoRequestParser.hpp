
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
Parser<ClientInfoRequest>::Parser() 
    : m_upClientInfoRequest(Tools::MakeUnique<ClientInfoRequest>()) {};

ClientInfoRequest& Parser<ClientInfoRequest>::GetObject()
{ return *m_upClientInfoRequest; };


inline std::string::const_iterator Parser<ClientInfoRequest>::FromString(const std::string& str)
{ return FromString(str.cbegin(), str.cend()); }

std::string::const_iterator Parser<ClientInfoRequest>::FromString(std::string::const_iterator begin, std::string::const_iterator end)
{
    Tools::type_integrity_assert<ClientInfoRequest, COMPILER_AWARE_VALUE(16, 16, 16) + sizeof(std::unique_ptr<ClientInfo>)>();
    if(std::distance(begin, end) < static_cast<std::ptrdiff_t>(sizeof(ERequestType)))
        return begin; //BAD INPUT. Range size has to be at least sizeof(ERequestType) bytes long

    ERequestType type;
    auto iter = ParseSeries(begin, end, type);
    if(std::distance(begin, iter) <= 0 ||
       type != ERequestType::ClientInfo)
            return begin;
    
    auto newIter = ParseSeries(iter, end, GetObject().GetClientInfo());

    if(std::distance(iter, newIter) <= 0)
        return begin;

    iter = OnFromString(newIter, end);
    if(newIter == end)
        return end;
    if(std::distance(newIter, iter) <= 0)
        return begin;
    return iter;
}

std::string::const_iterator inline Parser<ClientInfoRequest>::OnFromString(
    std::string::const_iterator,
    std::string::const_iterator end)
{ 
    return end; 
}
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------




}}  /*END OF NAMESPACES*/



#endif // _NICKSV_CHAT_CLIENTINFO_REQUEST_PARSER_PASTE