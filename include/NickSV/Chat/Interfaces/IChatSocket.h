
#ifndef _NICKSV_CHAT_ICHAT_SOCKET_T
#define _NICKSV_CHAT_ICHAT_SOCKET_T
#pragma once


#include "NickSV/Chat/Types.h"

#include <future>



namespace NickSV {
namespace Chat {


class IChatSocket
{
//FIXME Add const things and return EResult
public:
    [[nodiscard]]  virtual EResult Run(const ChatIPAddr &serverAddr) = 0;
    [[nodiscard]]  virtual    bool IsRunning()   = 0;
                   virtual    void CloseSocket() = 0;
                   virtual    void Wait()        = 0;
    //Queue request for sending
    [[nodiscard]]  virtual EResult QueueRequest( Request&, RequestInfo) = 0;
                   virtual EResult HandleRequest(Request&, RequestInfo) = 0;
                   virtual         ~IChatSocket() = default;
};







}}  /*END OF NAMESPACES*/

#endif // _NICKSV_CHAT_ICHAT_SOCKET_T