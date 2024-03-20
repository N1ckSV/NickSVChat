
#ifndef _NICKSV_CHAT_BSTRING_PARSER_PASTE
#define _NICKSV_CHAT_BSTRING_PARSER_PASTE
#pragma once




#include "NickSV/Chat/Defines.h"
#include "NickSV/Chat/Utils.h"

#include "NickSV/Chat/Parsers/BStringParser.h"




namespace NickSV::Chat {




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

    union
    {
        size_t Size = 0;
        char CharArr[sizeof(size_t)];
    } textSize;
    std::copy(begin, begin + sizeof(size_t), textSize.CharArr);
    auto iter = begin + sizeof(size_t);
    if(iter + sizeof(CharType) * textSize.Size > end)
        return begin; //BAD INPUT. Range size has to be at least sizeof(APIVersionType) + sizeof(size_t) bytes long
    
    union 
    {
        CharType* charType;
        char* Char;
    } pStr;
    GetObject()->resize(textSize.Size); 
    pStr.charType = GetObject()->data();
    std::copy(iter, iter + sizeof(CharType) * textSize.Size, pStr.Char);
    iter += sizeof(CharType) * textSize.Size;
    return iter;
}

template<typename CharT>
inline std::string::const_iterator Parser<std::basic_string<CharT>>::OnFromString(std::string::const_iterator begin, std::string::const_iterator end) 
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

} /*END OF NAMESPACES*/



#endif // _NICKSV_CHAT_BSTRING_PARSER_PASTE
