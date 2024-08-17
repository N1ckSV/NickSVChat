
#ifndef _NICKSV_CHAT_CLIENTINFO_REQUEST_T
#define _NICKSV_CHAT_CLIENTINFO_REQUEST_T
#pragma once

#include <memory>

#include "NickSV/Chat/ClientInfo.h"
#include "NickSV/Chat/Requests/Request.h"
#include "NickSV/Chat/Interfaces/ISerializable.h"



namespace NickSV {
namespace Chat {


class NICKSVCHAT_API ClientInfoRequest : public Request
{
    static_assert(is_serializable<ClientInfo>::value, is_serializable_ERROR_MESSAGE);
public:
    ClientInfoRequest();
    ClientInfoRequest(ClientInfo);

    ClientInfo& GetClientInfo();
    const ClientInfo& GetClientInfo() const;

    ERequestType GetType() const override;
    const std::unique_ptr<ISerializer> GetSerializer() const override;
private:
    std::unique_ptr<ClientInfo> m_upClientInfo;
};




}}  /*END OF NAMESPACES*/




#endif // _NICKSV_CHAT_CLIENTINFO_REQUEST_T