
#ifndef _NICKSV_CHAT_BMESSAGE_PARSER_PASTE
#define _NICKSV_CHAT_BMESSAGE_PARSER_PASTE
#pragma once



#include "NickSV/Chat/Definitions.h"
#include "NickSV/Chat/Utils.h"
#include "NickSV/Chat/Parsers/BStringParser.h"
#include "NickSV/Chat/Parsers/BMessageParser.h"




namespace NickSV {
namespace Chat {





//----------------------------------------------------------------------------------------------------
// Parser<BasicMessage<CharT>> implementation
//----------------------------------------------------------------------------------------------------
template<typename CharT>
Parser<BasicMessage<CharT>>::Parser() : m_upBasicMessage(new BasicMessage<CharT>()) {};

template<typename CharT>
BasicMessage<CharT>& Parser<BasicMessage<CharT>>::GetObject()
{ 
    return *m_upBasicMessage;
};

template<typename CharT>
inline std::string::const_iterator Parser<BasicMessage<CharT>>::FromString(const std::string& str)
{
    return FromString(str.cbegin(), str.cend());
}

template<typename CharT>
std::string::const_iterator Parser<BasicMessage<CharT>>::FromString(std::string::const_iterator begin, std::string::const_iterator end)
{
    constexpr size_t atleastSize = sizeof(UserIDType) + sizeof(size_t);
    Tools::type_integrity_assert<BasicMessage<CharT>, 
        COMPILER_AWARE_VALUE(8, 8, 8) +
        sizeof(UserIDType) + 
        sizeof(TextType)>();
        
    auto iter = ParseSeries(begin, end, GetObject().GetSenderID(), GetObject().GetText());
    if(std::distance(begin, iter) <= 0)
        return begin;

    auto newIter = OnFromString(iter, end);
    if(iter == end)
        return end;
    if(std::distance(iter, newIter) <= 0)
        return begin;
    return newIter;
}

template<typename CharT>
std::string::const_iterator inline Parser<BasicMessage<CharT>>::OnFromString(
    std::string::const_iterator,
    std::string::const_iterator end)
{ 
    return end; 
}
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------



}}  /*END OF NAMESPACES*/



#endif // _NICKSV_CHAT_BMESSAGE_PARSER_PASTE