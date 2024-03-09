#ifndef _NICKSV_CHAT_CLIENTINFO_PARSER_HEADER
#define _NICKSV_CHAT_CLIENTINFO_PARSER_HEADER
#pragma once


#include <string>
#include <memory>

#include "NickSV/Chat/Interfaces/IParser.h"
#include "NickSV/Chat/Utils.h"
#include "NickSV/Chat/ClientInfo.h"


namespace NickSV::Chat {



template<>
class Parser<ClientInfo> : public IParser
{
public:
    using UserIDType = ClientInfo::UserIDType;
    using APIVersionType = ClientInfo::APIVersionType;

    Parser();
    
    std::unique_ptr<ClientInfo>& GetObject();

    std::string::const_iterator FromString(const std::string&) override final;
    std::string::const_iterator FromString(std::string::const_iterator, std::string::const_iterator) override final;
    std::string::const_iterator OnFromString(std::string::const_iterator, std::string::const_iterator) override;
private:
    std::unique_ptr<ClientInfo> m_upClientInfo; //FIXME maybe const is redundant
};




} /*END OF NAMESPACES*/



#endif // _NICKSV_CHAT_CLIENTINFO_PARSER_HEADER