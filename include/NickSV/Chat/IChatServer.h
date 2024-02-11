
#ifndef _CHAT_ICHAT_SERVER_T
#define _CHAT_ICHAT_SERVER_T

#include <steam/steamnetworkingsockets.h>


#include "NickSV/Chat/Defines.h"


namespace NickSV::Chat {

    
class IChatServer
{
public:
    virtual void Run(const SteamNetworkingIPAddr &serverAddr ) = 0;
    virtual void CloseConnection() = 0;

    virtual ~IChatServer(){};
};







} /*END OF NAMESPACES*/

#endif // _CHAT_ICHAT_SERVER_T