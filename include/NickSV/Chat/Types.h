

#ifndef _CHAT_TYPES
#define _CHAT_TYPES



#include "NickSV/Chat/Defines.h"


namespace NickSV::Chat {

enum EResult
{
    kSucces = 0,
    kError
};

class IChatClient;
class ChatClient;
class IChatServer;
class ChatServer;

template<class CharT = TCHAR>
class ClientInfo;

template<typename Serializable>
class Serializer;

template<typename CharT = TCHAR>
using ClientInfoSerializer = Serializer<ClientInfo<CharT>>;



} /*END OF NAMESPACES*/



#endif // _CHAT_TYPES