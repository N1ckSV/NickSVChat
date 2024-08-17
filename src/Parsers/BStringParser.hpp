
#ifndef _NICKSV_CHAT_BSTRING_PARSER_PASTE
#define _NICKSV_CHAT_BSTRING_PARSER_PASTE
#pragma once




#include "NickSV/Chat/Definitions.h"
#include "NickSV/Chat/Utils.h"

#include "NickSV/Chat/Parsers/BStringParser.h"




namespace NickSV {
namespace Chat {




//----------------------------------------------------------------------------------------------------
// Parser<std::basic_string<CharT>> implementation
//---------------------------------------------------------------------------------------------------
template<typename CharT>
Parser<std::basic_string<CharT>>::Parser(): m_upBasicString(new std::basic_string<CharT>()) {};

template<typename CharT>
std::basic_string<CharT>& Parser<std::basic_string<CharT>>::GetObject()
{ 
    return *m_upBasicString;
};

template<typename CharT>
inline std::string::const_iterator Parser<std::basic_string<CharT>>::FromString(const std::string& str) 
{
    return FromString(str.cbegin(), str.cend());
}

template<typename CharT>
std::string::const_iterator Parser<std::basic_string<CharT>>::FromString(std::string::const_iterator begin, std::string::const_iterator end) 
{
    if(std::distance(begin, end) < static_cast<std::ptrdiff_t>(sizeof(size_t)))
        return begin; //BAD INPUT. Range size has to be at least sizeof(size_t) bytes long

    size_t textSize;
    auto iter = ParseSeries(begin, end, textSize);
    if(std::distance(begin, iter) <= 0 ||
       std::distance(iter, end) < static_cast<std::ptrdiff_t>(sizeof(CharType) * textSize))
        return begin;
    
    union {
        CharType* charType;
        char* Char;
    } pStr;
    GetObject().resize(textSize); 
    pStr.charType = GetObject().data();
    std::copy(iter, iter + sizeof(CharType) * textSize, pStr.Char);
    iter += sizeof(CharType) * textSize;
    return iter;
}

template<typename CharT>
inline std::string::const_iterator Parser<std::basic_string<CharT>>::OnFromString(
    std::string::const_iterator, 
    std::string::const_iterator end) 
{ 
    return end; 
};
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------


}}  /*END OF NAMESPACES*/



#endif // _NICKSV_CHAT_BSTRING_PARSER_PASTE
