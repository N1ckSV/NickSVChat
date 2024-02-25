

#include <iostream>

#include <steam/steamnetworkingsockets.h>

#include "NickSV/Chat/ChatClient.h"
#include "NickSV/Chat/BasicClientInfo.h"


namespace NickSV::Chat {

EResult ChatClient::Run(const SteamNetworkingIPAddr &serverAddr, ChatErrorMsg &errMsg )
{
	if(IsRunning() || m_pConnectionThread)
	{
		CHAT_EXPECT(false, "Connection is already running");
		return EResult::AlreadyRunning;
	}

	EResult result = this->OnPreStartConnection(serverAddr, errMsg);
    if(result != EResult::Success)
		return result;

    ///////////////////////////////////
	if (!GameNetworkingSockets_Init( nullptr, errMsg ))
        return EResult::Error;

    m_pInterface = SteamNetworkingSockets();
	SteamNetworkingConfigValue_t opt;
	opt.SetPtr( k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged, (void*)SteamNetConnectionStatusChangedCallback );
    m_hConnection = m_pInterface->ConnectByIPAddress( serverAddr, 1,&opt);    

    if (m_hConnection == k_HSteamNetConnection_Invalid)
	{
		GameNetworkingSockets_Kill();
		sprintf(errMsg, "ISteamNetworkingSockets interface returned invalid connection");
        return EResult::Error;
	}

    m_bGoingExit = false; //SHOULD BE ALWAYS FIRST BEFORE m_pConnectionThread = new ...
    m_pConnectionThread = new std::thread(&ChatClient::ConnectionThreadFunction, this);
    //TODO: exit/kill/nuke/terminate

    ///////////////////////////////////
    return this->OnStartConnection(serverAddr, errMsg);
}

void ChatClient::SetInfo(BasicClientInfo<>* pChatClientInfo)
{
	
}

void ChatClient::CloseConnection()
{
    CHAT_EXPECT(m_pConnectionThread, "Connection thread is missing (m_pConnectionThread == null).");

    this->OnPreCloseConnection();
    ///////////////////////////////////
    //TODO
    //FIXME FIXME FIXME FIXME FIXME FIXME
    m_bGoingExit = true;
	if(m_pConnectionThread)
   		m_pConnectionThread->join();

    delete m_pConnectionThread;
    m_pConnectionThread = nullptr;
    ///////////////////////////////////
    this->OnCloseConnection();
}

bool inline ChatClient::IsRunning() { return !m_bGoingExit; }

ChatClient::ChatClient()
{
    m_bGoingExit = true;
    m_pConnectionThread = nullptr;
	m_pClientInfo = new ClientInfo();
	/////////
}

ChatClient::~ChatClient()
{
    //TODO
    //FIXME FIXME FIXME FIXME FIXME FIXME 
	delete m_pClientInfo;
    delete m_pConnectionThread;
}

EResult ChatClient::OnPreStartConnection(const SteamNetworkingIPAddr &serverAddr, ChatErrorMsg &errMsg)  { return EResult::Success; };
EResult ChatClient::OnStartConnection(const SteamNetworkingIPAddr &serverAddr, ChatErrorMsg &errMsg)     { return EResult::Success; };
void ChatClient::OnPreCloseConnection()  { };
void ChatClient::OnCloseConnection()     { };

void ChatClient::ConnectionThreadFunction()
{
    while(!m_bGoingExit)
    {
		s_pCallbackInstance = this;
		m_pInterface->RunCallbacks();
        ISteamNetworkingMessage *pIncomingMsg = nullptr;
		int numMsgs = m_pInterface->ReceiveMessagesOnConnection( m_hConnection, &pIncomingMsg, 1 );
		if ( numMsgs == 0 )
			continue;

		// Just echo anything we get from the server
		std::string str = std::string((const char*)pIncomingMsg->m_pData, std::size_t(pIncomingMsg->m_cbSize));
		std::cout << str; 

		// We don't need this anymore.
		pIncomingMsg->Release();
    }
	m_pInterface->CloseConnection( m_hConnection, 0, "Closing con", true );
}

ChatClient *ChatClient::s_pCallbackInstance = nullptr;

void ChatClient::SteamNetConnectionStatusChangedCallback( SteamNetConnectionStatusChangedCallback_t *pInfo )
{
	s_pCallbackInstance->OnSteamNetConnectionStatusChanged( pInfo );
}

void ChatClient::OnSteamNetConnectionStatusChanged( SteamNetConnectionStatusChangedCallback_t *pInfo )
{	
	CHAT_ASSERT( pInfo->m_hConn == m_hConnection || m_hConnection == k_HSteamNetConnection_Invalid, "" );

	// What's the state of the connection?
	switch ( pInfo->m_info.m_eState )
	{
		case k_ESteamNetworkingConnectionState_None:
			// NOTE: We will get callbacks here when we destroy connections.  You can ignore these.
			break;

		case k_ESteamNetworkingConnectionState_ClosedByPeer:
		case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
		{
			m_bGoingExit = true;

			// Print an appropriate message
			if ( pInfo->m_eOldState == k_ESteamNetworkingConnectionState_Connecting )
			{
				// Note: we could distinguish between a timeout, a rejected connection,
				// or some other transport problem.
				//Printf( "We sought the remote host, yet our efforts were met with defeat.  (%s)", pInfo->m_info.m_szEndDebug );
			}
			else if ( pInfo->m_info.m_eState == k_ESteamNetworkingConnectionState_ProblemDetectedLocally )
			{
				//Printf( "Alas, troubles beset us; we have lost contact with the host.  (%s)", pInfo->m_info.m_szEndDebug );
			}
			else
			{
				// NOTE: We could check the reason code for a normal disconnection
				//Printf( "The host hath bidden us farewell.  (%s)", pInfo->m_info.m_szEndDebug );
			}

			// Clean up the connection.  This is important!
			// The connection is "closed" in the network sense, but
			// it has not been destroyed.  We must close it on our end, too
			// to finish up.  The reason information do not matter in this case,
			// and we cannot linger because it's already closed on the other end,
			// so we just pass 0's.
			m_pInterface->CloseConnection( pInfo->m_hConn, 0, nullptr, false );
			m_hConnection = k_HSteamNetConnection_Invalid;
			break;
		}

		case k_ESteamNetworkingConnectionState_Connecting:
			// We will get this callback when we start connecting.
			// We can ignore this.
			break;

		case k_ESteamNetworkingConnectionState_Connected:
			//Printf( "Connected to server OK" );
			break;

		default:
			// Silences -Wswitch
			break;
	}
}
	




} /*END OF NAMESPACES*/