#ifndef _NICKSV_CHAT_MESSAGE_PARSER_HEADER
#define _NICKSV_CHAT_MESSAGE_PARSER_HEADER
#pragma once


#include <string>
#include <memory>

#include "NickSV/Chat/Interfaces/IParser.h"
#include "NickSV/Chat/Utils.h"
#include "NickSV/Chat/Message.h"


namespace NickSV {
namespace Chat {


template<>
class NICKSVCHAT_API Parser<Message> : public IParser
{
public:
    using CharType = typename Message::CharType;
    using TextType = typename Message::TextType;
    using UserIDType = typename Message::UserIDType;

    Parser(std::unique_ptr<Message> upMessage = Tools::MakeUnique<Message>());
    
    Message& GetObject();

    std::string::const_iterator FromString(const std::string&) override final;
    std::string::const_iterator FromString(std::string::const_iterator, std::string::const_iterator) override final;
    std::string::const_iterator OnFromString(std::string::const_iterator, std::string::const_iterator) override;
protected:
     std::unique_ptr<Message>  m_upMessage;
};



}}  /*END OF NAMESPACES*/



#endif // _NICKSV_CHAT_MESSAGE_PARSER_HEADER