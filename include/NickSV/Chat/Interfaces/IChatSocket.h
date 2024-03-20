
#ifndef _NICKSV_CHAT_ICHAT_SOCKET_T
#define _NICKSV_CHAT_ICHAT_SOCKET_T
#pragma once


#include "NickSV/Chat/Types.h"




namespace NickSV::Chat {


class IChatSocket
{
//FIXME Add const things and return EResult
public:
    CHAT_NODISCARD virtual EResult Run(const ChatIPAddr &serverAddr) = 0;
    CHAT_NODISCARD virtual    bool IsRunning()   = 0;
                   virtual    void CloseSocket() = 0;
                   virtual    void Wait()        = 0;
    //Queue request for sending
    CHAT_NODISCARD virtual EResult QueueRequest( Request*, RequestInfo) = 0;
    CHAT_NODISCARD virtual EResult HandleRequest(Request*, RequestInfo) = 0;
                   virtual         ~IChatSocket() = default;
};







} /*END OF NAMESPACES*/

#endif // _NICKSV_CHAT_ICHAT_SOCKET_T