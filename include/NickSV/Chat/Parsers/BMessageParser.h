#ifndef _NICKSV_CHAT_BMESSAGE_PARSER_HEADER
#define _NICKSV_CHAT_BMESSAGE_PARSER_HEADER
#pragma once


#include <string>
#include <memory>

#include "NickSV/Chat/Interfaces/IParser.h"
#include "NickSV/Chat/Utils.h"
#include "NickSV/Chat/BasicMessage.h"


namespace NickSV::Chat {


template<typename CharT>
class Parser<BasicMessage<CharT>> : public IParser
{
public:
    using CharType = typename BasicMessage<CharT>::CharType;

    Parser();
    
    std::unique_ptr<BasicMessage<CharT>>& GetObject();

    std::string::const_iterator FromString(const std::string&) override final;
    std::string::const_iterator FromString(std::string::const_iterator, std::string::const_iterator) override final;
    std::string::const_iterator OnFromString(std::string::const_iterator, std::string::const_iterator) override;
private:
     std::unique_ptr<BasicMessage<CharT>>  m_upBasicMessage; //FIXME maybe second const is redundant
};



} /*END OF NAMESPACES*/



#endif // _NICKSV_CHAT_BMESSAGE_PARSER_HEADER