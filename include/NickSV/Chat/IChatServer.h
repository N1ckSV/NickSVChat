
#ifndef _NICKSV_CHAT_ICHAT_SERVER_T
#define _NICKSV_CHAT_ICHAT_SERVER_T
#pragma once

#include "NickSV/Chat/Types.h"


namespace NickSV::Chat {

    
class IChatServer
{
public:
    virtual void Run(const SteamNetworkingIPAddr &serverAddr ) = 0;
    virtual void CloseConnection() = 0;

    virtual ~IChatServer(){};
};







} /*END OF NAMESPACES*/

#endif // _NICKSV_CHAT_ICHAT_SERVER_T