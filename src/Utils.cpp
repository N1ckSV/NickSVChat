
#include "NickSV/Chat/Utils.h"

#include <steam/isteamnetworkingutils.h>

namespace NickSV::Chat {



ChatIPAddr::ChatIPAddr()
{
    ParseString("127.0.0.1");
    m_port = DEFAULT_PORT;
}

//ChatIPAddr::operator SteamNetworkingIPAddr() const
//{
//    SteamNetworkingIPAddr addr;
//    addr.SetIPv6(m_ipv6, m_port);
//    return addr;
//}


} /*END OF NAMESPACES*/