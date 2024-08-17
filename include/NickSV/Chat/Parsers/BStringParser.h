#ifndef _NICKSV_CHAT_BSTRING_PARSER_HEADER
#define _NICKSV_CHAT_BSTRING_PARSER_HEADER
#pragma once


#include <string>
#include <memory>


#include "NickSV/Chat/Interfaces/IParser.h"
#include "NickSV/Chat/Utils.h"


namespace NickSV {
namespace Chat {


template<typename CharT>
class NICKSVCHAT_API Parser<std::basic_string<CharT>> final : public IParser
{
public:
    using CharType = CharT;

    Parser();
    
    std::basic_string<CharT>& GetObject();

    std::string::const_iterator FromString(const std::string&) override;
    std::string::const_iterator FromString(std::string::const_iterator, std::string::const_iterator) override;
    std::string::const_iterator OnFromString(std::string::const_iterator, std::string::const_iterator) override;
private:
     std::unique_ptr<std::basic_string<CharT>>  m_upBasicString;
};


}}  /*END OF NAMESPACES*/



#endif // _NICKSV_CHAT_BSTRING_PARSER_HEADER