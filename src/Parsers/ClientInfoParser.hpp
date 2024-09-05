
#ifndef _NICKSV_CHAT_CLIENTINFO_PARSER_PASTE
#define _NICKSV_CHAT_CLIENTINFO_PARSER_PASTE
#pragma once


#include "NickSV/Chat/Definitions.h"
#include "NickSV/Chat/Utils.h"
#include "NickSV/Chat/Parsers/ClientInfoParser.h"

#include "NickSV/Tools/TypeTraits.h"




namespace NickSV {
namespace Chat {





//----------------------------------------------------------------------------------------------------
// Parser<ClientInfo> implementation
//----------------------------------------------------------------------------------------------------

Parser<ClientInfo>::Parser(std::unique_ptr<ClientInfo> upClientInfo) 
    : m_upClientInfo(std::move(upClientInfo)) {};

ClientInfo& Parser<ClientInfo>::GetObject()
{ return *m_upClientInfo; };


inline std::string::const_iterator Parser<ClientInfo>::FromString(const std::string& str)
{ return FromString(str.cbegin(), str.cend()); }


std::string::const_iterator Parser<ClientInfo>::FromString(std::string::const_iterator begin, std::string::const_iterator end)
{
    constexpr size_t atleastSize = sizeof(LibVersionType) + sizeof(EState) + sizeof(UserIDType);
    Tools::type_integrity_assert<ClientInfo, atleastSize + COMPILER_AWARE_VALUE(8, 8, 8)>();
    if(std::distance(begin, end) < static_cast<std::ptrdiff_t>(atleastSize))
        return begin; //BAD INPUT. Range size has to be atleastSize bytes long

    auto iter = ParseSeries(
        begin, end, 
        GetObject().GetLibVer(), 
        GetObject().GetState(), 
        GetObject().GetUserID());

    if(std::distance(begin, iter) <= 0)
        return begin;

    auto newIter = OnFromString(iter, end);
    if(iter == end)
        return end;
    if(std::distance(iter, newIter) <= 0)
        return begin;
    return newIter;
}


std::string::const_iterator inline Parser<ClientInfo>::OnFromString(
    std::string::const_iterator,
    std::string::const_iterator end)
{ 
    return end; 
}
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------




}}  /*END OF NAMESPACES*/



#endif // _NICKSV_CHAT_CLIENTINFO_PARSER_PASTE