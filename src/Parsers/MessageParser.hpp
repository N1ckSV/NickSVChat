
#ifndef _NICKSV_CHAT_MESSAGE_PARSER_PASTE
#define _NICKSV_CHAT_MESSAGE_PARSER_PASTE
#pragma once



#include "NickSV/Chat/Parsers/BStringParser.h"
#include "NickSV/Chat/Parsers/MessageParser.h"




namespace NickSV {
namespace Chat {





//----------------------------------------------------------------------------------------------------
// Parser<Message> implementation
//----------------------------------------------------------------------------------------------------
Parser<Message>::Parser(std::unique_ptr<Message> upMessage) 
    : m_upMessage(std::move(upMessage)) {};

Message& Parser<Message>::GetObject()
{ return *m_upMessage; };

inline std::string::const_iterator Parser<Message>::FromString(const std::string& str)
{ return FromString(str.cbegin(), str.cend()); }

std::string::const_iterator Parser<Message>::FromString(std::string::const_iterator begin, std::string::const_iterator end)
{
    constexpr size_t atleastSize = sizeof(UserIDType) + sizeof(size_t);
    Tools::type_integrity_assert<Message, 
        COMPILER_AWARE_VALUE(8, 8, 8) +
        sizeof(UserIDType) + 
        sizeof(TextType)>();
        
    auto iter = ParseSeries(
        begin, end, 
        GetObject().GetSenderID(), 
        GetObject().GetText());

    if(std::distance(begin, iter) <= 0)
        return begin;

    auto newIter = OnFromString(iter, end);
    if(iter == end)
        return end;
    if(std::distance(iter, newIter) <= 0)
        return begin;
    return newIter;
}

std::string::const_iterator inline Parser<Message>::OnFromString(
    std::string::const_iterator,
    std::string::const_iterator end)
{ 
    return end; 
}
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------



}}  /*END OF NAMESPACES*/



#endif // _NICKSV_CHAT_MESSAGE_PARSER_PASTE