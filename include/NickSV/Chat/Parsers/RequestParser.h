#ifndef _NICKSV_CHAT_REQUEST_PARSER_HEADER
#define _NICKSV_CHAT_REQUEST_PARSER_HEADER
#pragma once


#include <string>
#include <memory>

#include "NickSV/Chat/Interfaces/IParser.h"
#include "NickSV/Chat/Utils.h"


namespace NickSV::Chat {




/*
 This comment is case sensitive for the words "Request" and "request",
 "Request" is a class, "request" is an internet request.


 Request is class that represens "unknown request" and when you parsing it
 you are actually want to parse and get other requests that are derived from Request.
 And if you got unique_ptr to Request object (GetType() == ERequestType::Unknown) after
 parsing string insteed of ClientInfoRequest (GetType() == ERequestType::ClientInfo)
 or MessageRequest (GetType() == ERequestType::Message) etc, that means
 something went wrong and string or code are broken.
 Note also that Request has no Serializer, cause you don't need to serialize
 "unknown request", all requests you serialize to send should be known.
*/

template<>
class Parser<Request> : public IParser
{
public:
    Parser();
    
    std::unique_ptr<Request>& GetObject();

    virtual std::unique_ptr<Parser<ClientInfoRequest>> GetClientInfoRequestParser();
    virtual std::unique_ptr<Parser<MessageRequest>>    GetMessageRequestParser();

    std::string::const_iterator FromString(const std::string&) override final;
    std::string::const_iterator FromString(std::string::const_iterator, std::string::const_iterator) override final;
    std::string::const_iterator OnFromString(std::string::const_iterator, std::string::const_iterator) override;
private:
    std::unique_ptr<Request>  m_upRequest; //FIXME maybe const is redundant
};



} /*END OF NAMESPACES*/



#endif // _NICKSV_CHAT_REQUEST_PARSER_HEADER