
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
Parser<BasicMessage<CharT>>::Parser() : m_upBasicMessage(std::make_unique<BasicMessage<CharT>>()) {};

template<typename CharT>
std::unique_ptr<BasicMessage<CharT>>& Parser<BasicMessage<CharT>>::GetObject()
{ 
    return m_upBasicMessage;
};

template<typename CharT>
inline std::string::const_iterator Parser<BasicMessage<CharT>>::FromString(const std::string& str)
{
    return FromString(str.cbegin(), str.cend());
}

template<typename CharT>
std::string::const_iterator Parser<BasicMessage<CharT>>::FromString(std::string::const_iterator begin, std::string::const_iterator end)
{
    constexpr size_t atleastSize = sizeof(size_t);
    Tools::type_integrity_assert<BasicMessage<CharT>, COMPILER_AWARE_VALUE(8, 8, 8) + sizeof(typename BasicMessage<CharT>::TextType)>();
    if(begin + atleastSize > end)
        return begin; //BAD INPUT. Range size has to be atleastSize bytes long
        
    auto parser = Parser<std::basic_string<CharT>>();
    auto iter = parser.FromString(begin, end);
    if(iter <= begin)
        return begin; //BAD INPUT. Parsed size doesnt match with input size

    std::swap(GetObject()->GetText(), *parser.GetObject());
    return OnFromString(iter, end);
}

template<typename CharT>
std::string::const_iterator inline Parser<BasicMessage<CharT>>::OnFromString(
    std::string::const_iterator begin,
    std::string::const_iterator)
{ 
    return begin; 
}
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------


template class Parser<BasicMessage<char>>;
template class Parser<BasicMessage<wchar_t>>;
template class Parser<BasicMessage<char16_t>>;
template class Parser<BasicMessage<char32_t>>;


}}  /*END OF NAMESPACES*/



#endif // _NICKSV_CHAT_BMESSAGE_PARSER_PASTE