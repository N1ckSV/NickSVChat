
#ifndef _NICKSV_CHAT_ICHAT_SOCKET_T
#define _NICKSV_CHAT_ICHAT_SOCKET_T
#pragma once

#include "NickSV/Chat/Types.h"


namespace NickSV::Chat {


class IChatSocket
{
public:
    CHAT_NODISCARD virtual EResult Run(const SteamNetworkingIPAddr &serverAddr, ChatErrorMsg &errMsg  ) = 0;
    virtual void CloseConnection() = 0;
    virtual void SendRequest(const IRequest*) = 0;
    virtual ~IChatSocket() = default;
};







} /*END OF NAMESPACES*/

#endif // _NICKSV_CHAT_ICHAT_SOCKET_T