
#ifndef _NICKSV_CHAT_CLIENTINFO_PARSER_PASTE
#define _NICKSV_CHAT_CLIENTINFO_PARSER_PASTE
#pragma once


#include "NickSV/Chat/Defines.h"
#include "NickSV/Chat/Utils.h"
#include "NickSV/Chat/Parsers/ClientInfoParser.h"




namespace NickSV::Chat {





//----------------------------------------------------------------------------------------------------
// Parser<ClientInfo> implementation
//----------------------------------------------------------------------------------------------------

Parser<ClientInfo>::Parser() : m_upClientInfo(std::make_unique<ClientInfo>()) {};


std::unique_ptr<ClientInfo>& Parser<ClientInfo>::GetObject()
{ 
    return m_upClientInfo;
};


inline std::string::const_iterator Parser<ClientInfo>::FromString(const std::string& str)
{
    return FromString(str.cbegin(), str.cend());
}


std::string::const_iterator Parser<ClientInfo>::FromString(std::string::const_iterator begin, std::string::const_iterator end)
{
    constexpr size_t atleastSize = sizeof(APIVersionType) + sizeof(EState) + sizeof(UserIDType);
    type_integrity_assert<ClientInfo, atleastSize + 8>();
    if(begin + atleastSize > end)
        return begin; //BAD INPUT. Range size has to be atleastSize bytes long

    Transfer<APIVersionType> ver;
    std::copy(begin, begin + sizeof(APIVersionType), ver.CharArr);
    auto iter = begin + sizeof(APIVersionType);
    GetObject()->GetAPIVer() = ver.Base;
    Transfer<EState> state;
    std::copy(iter, iter + sizeof(EState), state.CharArr);
    iter += sizeof(EState);
    GetObject()->GetState() = state.Base;
    Transfer<UserIDType> id;
    std::copy(iter, iter + sizeof(UserIDType), id.CharArr);
    iter += sizeof(UserIDType);
    GetObject()->GetUserID() = id.Base;

    return OnFromString(iter, end);
}


std::string::const_iterator inline Parser<ClientInfo>::OnFromString(std::string::const_iterator begin, std::string::const_iterator end)
{ 
    return begin; 
}
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------



template class Parser<ClientInfo>;


} /*END OF NAMESPACES*/



#endif // _NICKSV_CHAT_CLIENTINFO_PARSER_PASTE