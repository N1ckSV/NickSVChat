
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
Parser<std::basic_string<CharT>>::Parser(): m_upBasicString(std::make_unique<std::basic_string<CharT>>()) {};

template<typename CharT>
std::unique_ptr<std::basic_string<CharT>>& Parser<std::basic_string<CharT>>::GetObject()
{ 
    return m_upBasicString;
};

template<typename CharT>
inline std::string::const_iterator Parser<std::basic_string<CharT>>::FromString(const std::string& str) 
{
    return FromString(str.cbegin(), str.cend());
}

template<typename CharT>
std::string::const_iterator Parser<std::basic_string<CharT>>::FromString(std::string::const_iterator begin, std::string::const_iterator end) 
{
    if(begin + sizeof(size_t) > end)
        return begin; //BAD INPUT. Range size has to be at least sizeof(size_t) bytes long

    Transfer<size_t> textSize;
    std::copy(begin, begin + sizeof(size_t), textSize.CharArr);
    auto iter = begin + sizeof(size_t);
    if(iter + sizeof(CharType) * textSize.Base > end)
        return begin; //BAD INPUT. Range size has to be at least sizeof(LibVersionType) + sizeof(size_t) bytes long
    
    union 
    {
        CharType* charType;
        char* Char;
    } pStr;
    GetObject()->resize(textSize.Base); 
    pStr.charType = GetObject()->data();
    std::copy(iter, iter + sizeof(CharType) * textSize.Base, pStr.Char);
    iter += sizeof(CharType) * textSize.Base;
    return iter;
}

template<typename CharT>
inline std::string::const_iterator Parser<std::basic_string<CharT>>::OnFromString(
    std::string::const_iterator begin, 
    std::string::const_iterator) 
{ 
    return begin; 
};
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------


template class Parser<std::basic_string<char>>;
template class Parser<std::basic_string<wchar_t>>;
template class Parser<std::basic_string<char16_t>>;
template class Parser<std::basic_string<char32_t>>;

}}  /*END OF NAMESPACES*/



#endif // _NICKSV_CHAT_BSTRING_PARSER_PASTE
