

#ifndef _CHAT_ICHAT_CLIENT_T
#define _CHAT_ICHAT_CLIENT_T

#include <steam/steamnetworkingsockets.h>


#include "NickSV/Chat/Defines.h"


namespace NickSV::Chat {

    
class IChatClient
{
public:
    virtual void Run(const SteamNetworkingIPAddr &serverAddr ) = 0;
    virtual void CloseConnection() = 0;

    virtual ~IChatClient(){};
};







} /*END OF NAMESPACES*/

#endif // _CHAT_ICHAT_CLIENT_T