
#ifndef _NICKSV_CHAT_CLIENTINFO_REQUEST_PASTE
#define _NICKSV_CHAT_CLIENTINFO_REQUEST_PASTE
#pragma once

#include "NickSV/Chat/Serializers/ClientInfoRequestSerializer.h"
#include "NickSV/Chat/Parsers/ClientInfoRequestParser.h"
#include "NickSV/Chat/Requests/ClientInfoRequest.h"

namespace NickSV {
namespace Chat {


//-----------------------------------------------------------------------------------
// Impementation of ClientInfoRequest
//-----------------------------------------------------------------------------------
ClientInfoRequest::ClientInfoRequest() : m_upClientInfo(Tools::MakeUnique<ClientInfo>()) {};

ClientInfoRequest::ClientInfoRequest(ClientInfo clientInfo) : m_upClientInfo(Tools::MakeUnique<ClientInfo>(std::move(clientInfo))) {};

ClientInfo& ClientInfoRequest::GetClientInfo()
{
    return *m_upClientInfo;
}

const ClientInfo& ClientInfoRequest::GetClientInfo() const
{
    return *m_upClientInfo;
}

ERequestType ClientInfoRequest::GetType() const { return ERequestType::ClientInfo; }

inline auto ClientInfoRequest::GetSerializer() const
-> const std::unique_ptr<ISerializer>
{ 
	return Tools::MakeUnique<Serializer<ClientInfoRequest>>(*this);
}

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------




}}  /*END OF NAMESPACES*/




#endif // _NICKSV_CHAT_BCLIENTINFO_REQUEST_PASTE