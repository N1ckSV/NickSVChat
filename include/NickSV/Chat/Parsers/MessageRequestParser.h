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
class NICKSVCHAT_API Parser<MessageRequest> : public IParser
{
public:
    Parser();
    
    MessageRequest& GetObject();

    std::string::const_iterator FromString(const std::string&) override final;
    std::string::const_iterator FromString(std::string::const_iterator, std::string::const_iterator) override final;
    std::string::const_iterator OnFromString(std::string::const_iterator, std::string::const_iterator) override;

    friend Parser<Request>;
protected:
    std::unique_ptr<MessageRequest>  m_upMessageRequest;
};



}}  /*END OF NAMESPACES*/



#endif // _NICKSV_CHAT_MESSAGE_REQUEST_PARSER_HEADER