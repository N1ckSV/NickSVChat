
#ifndef _NICKSV_CHAT_ICHAT_CLIENT_T
#define _NICKSV_CHAT_ICHAT_CLIENT_T
#pragma once

#include "NickSV/Chat/Types.h"


namespace NickSV::Chat {

    
class IChatClient
{
public:
    virtual void Run(const SteamNetworkingIPAddr &serverAddr ) = 0;
    virtual void CloseConnection() = 0;

    virtual ~IChatClient(){};
};







} /*END OF NAMESPACES*/

#endif // _NICKSV_CHAT_ICHAT_CLIENT_T