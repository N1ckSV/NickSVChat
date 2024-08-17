#ifndef _NICKSV_CHAT_BMESSAGE_PARSER_HEADER
#define _NICKSV_CHAT_BMESSAGE_PARSER_HEADER
#pragma once


#include <string>
#include <memory>

#include "NickSV/Chat/Interfaces/IParser.h"
#include "NickSV/Chat/Utils.h"
#include "NickSV/Chat/BasicMessage.h"


namespace NickSV {
namespace Chat {


template<typename CharT>
class NICKSVCHAT_API Parser<BasicMessage<CharT>> : public IParser
{
public:
    using CharType = typename BasicMessage<CharT>::CharType;
    using TextType = typename BasicMessage<CharT>::TextType;
    using UserIDType = typename BasicMessage<CharT>::UserIDType;

    Parser();
    
    BasicMessage<CharT>& GetObject();

    std::string::const_iterator FromString(const std::string&) override final;
    std::string::const_iterator FromString(std::string::const_iterator, std::string::const_iterator) override final;
    std::string::const_iterator OnFromString(std::string::const_iterator, std::string::const_iterator) override;
protected:
     std::unique_ptr<BasicMessage<CharT>>  m_upBasicMessage;
};



}}  /*END OF NAMESPACES*/



#endif // _NICKSV_CHAT_BMESSAGE_PARSER_HEADER