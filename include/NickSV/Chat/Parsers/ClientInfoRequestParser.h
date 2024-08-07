#ifndef _NICKSV_CHAT_CLIENTINFO_REQUEST_PARSER_HEADER
#define _NICKSV_CHAT_CLIENTINFO_REQUEST_PARSER_HEADER
#pragma once


#include <string>
#include <memory>

#include "NickSV/Chat/Interfaces/IParser.h"
#include "NickSV/Chat/Utils.h"
#include "NickSV/Chat/ClientInfo.h"


namespace NickSV {
namespace Chat {




template<>
class Parser<ClientInfoRequest> : public IParser
{
public:
    Parser();
    
    std::unique_ptr<ClientInfoRequest>& GetObject();
    
    virtual std::unique_ptr<Parser<ClientInfo>> GetClientInfoParser();

    std::string::const_iterator FromString(const std::string&) override final;
    std::string::const_iterator FromString(std::string::const_iterator, std::string::const_iterator) override final;
    std::string::const_iterator OnFromString(std::string::const_iterator, std::string::const_iterator) override;
private:
    std::unique_ptr<ClientInfoRequest>  m_upClientInfoRequest;
};



}}  /*END OF NAMESPACES*/



#endif // _NICKSV_CHAT_CLIENTINFO_REQUEST_PARSER_HEADER