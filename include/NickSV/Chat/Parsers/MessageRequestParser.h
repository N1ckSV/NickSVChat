#ifndef _NICKSV_CHAT_MESSAGE_REQUEST_PARSER_HEADER
#define _NICKSV_CHAT_MESSAGE_REQUEST_PARSER_HEADER
#pragma once


#include <string>
#include <memory>

#include "NickSV/Chat/Interfaces/IParser.h"
#include "NickSV/Chat/Utils.h"


namespace NickSV {
namespace Chat {




template<>
class Parser<MessageRequest> : public IParser
{
public:
    Parser();
    
    std::unique_ptr<MessageRequest>& GetObject();

    virtual std::unique_ptr<Parser<Message>> GetMessageParser();

    std::string::const_iterator FromString(const std::string&) override final;
    std::string::const_iterator FromString(std::string::const_iterator, std::string::const_iterator) override final;
    std::string::const_iterator OnFromString(std::string::const_iterator, std::string::const_iterator) override;
private:
    std::unique_ptr<MessageRequest>  m_upMessageRequest; //FIXME maybe const is redundant
};



}}  /*END OF NAMESPACES*/



#endif // _NICKSV_CHAT_MESSAGE_REQUEST_PARSER_HEADER