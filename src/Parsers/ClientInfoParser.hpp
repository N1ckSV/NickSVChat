
#ifndef _NICKSV_CHAT_CLIENTINFO_PARSER_PASTE
#define _NICKSV_CHAT_CLIENTINFO_PARSER_PASTE
#pragma once


#include "NickSV/Chat/Definitions.h"
#include "NickSV/Chat/Utils.h"
#include "NickSV/Chat/Parsers/ClientInfoParser.h"

#include "NickSV/Tools/TypeTraits.h"




namespace NickSV {
namespace Chat {





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
    constexpr size_t atleastSize = sizeof(LibVersionType) + sizeof(EState) + sizeof(UserIDType);
    Tools::type_integrity_assert<ClientInfo, atleastSize + COMPILER_AWARE_VALUE(8, 8, 8)>();
    if(begin + atleastSize > end)
        return begin; //BAD INPUT. Range size has to be atleastSize bytes long

    Transfer<LibVersionType> ver;
    std::copy(begin, begin + sizeof(LibVersionType), ver.CharArr);
    auto iter = begin + sizeof(LibVersionType);
    GetObject()->GetLibVer() = ver.Base;
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


std::string::const_iterator inline Parser<ClientInfo>::OnFromString(
    std::string::const_iterator begin,
    std::string::const_iterator)
{ 
    return begin; 
}
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------



template class Parser<ClientInfo>;


}}  /*END OF NAMESPACES*/



#endif // _NICKSV_CHAT_CLIENTINFO_PARSER_PASTE