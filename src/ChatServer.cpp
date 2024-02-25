

#include <iostream>

#include <steam/steamnetworkingsockets.h>

#include "NickSV/Chat/ChatServer.h"
#include "NickSV/Chat/BasicClientInfo.h"


namespace NickSV::Chat {

EResult ChatServer::Run(const SteamNetworkingIPAddr &serverAddr, ChatErrorMsg &errMsg)
{
    /*Description*/

    if(IsRunning() || m_pConnectionThread)
	{
		CHAT_EXPECT(false, "Connection is already running");
		return EResult::AlreadyRunning;
	}

	EResult result = this->OnPreStartConnection(serverAddr, errMsg );
    if(result != EResult::Success)
		return result;
		
    ///////////////////////////////////
	if (!GameNetworkingSockets_Init( nullptr, errMsg ))
        return EResult::Error;

    m_pInterface = SteamNetworkingSockets();
    SteamNetworkingConfigValue_t opt;
	opt.SetPtr( k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged, (void*)SteamNetConnectionStatusChangedCallback );
	m_hListenSock = m_pInterface->CreateListenSocketIP( serverAddr, 1, &opt );

    if ( m_hListenSock == k_HSteamListenSocket_Invalid )
            return EResult::Error;

	m_hPollGroup = m_pInterface->CreatePollGroup();

	if (m_hPollGroup == k_HSteamNetPollGroup_Invalid)
	{
		GameNetworkingSockets_Kill();
		sprintf(errMsg, "ISteamNetworkingSockets interface returned invalid connection");
        return EResult::Error;
	}

    m_bGoingExit = false; //SHOULD BE FIRST BEFORE m_pConnectionThread = new ...
    m_pConnectionThread = new std::thread(&ChatServer::ConnectionThreadFunction, this);
    //TODO: exit/kill/nuke/terminate

    ///////////////////////////////////
    return this->OnStartConnection(serverAddr, errMsg);
}

ChatServer *ChatServer::s_pCallbackInstance = nullptr;
void ChatServer::SteamNetConnectionStatusChangedCallback( SteamNetConnectionStatusChangedCallback_t *pInfo )
{
	s_pCallbackInstance->OnSteamNetConnectionStatusChanged( pInfo );
}

void ChatServer::CloseConnection()
{
    /*Description*/
    
	//ERROR: m_pConnectionThread is null.
    CHAT_EXPECT(m_pConnectionThread, "Conection thread is missing (m_pConnectionThread == null).");

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

bool inline ChatServer::IsRunning() { return !m_bGoingExit; }

ChatServer::ChatServer()
{
    m_bGoingExit = true;
    m_pConnectionThread = nullptr;
}
ChatServer::~ChatServer()
{
    //TODO
    //FIXME FIXME FIXME FIXME FIXME FIXME 
    delete m_pConnectionThread;
}

EResult ChatServer::OnPreStartConnection(const SteamNetworkingIPAddr &serverAddr, ChatErrorMsg &errMsg)  { return EResult::Success; }
EResult ChatServer::OnStartConnection(const SteamNetworkingIPAddr &serverAddr, ChatErrorMsg &errMsg)     { return EResult::Success; }
void ChatServer::OnPreCloseConnection()  { }
void ChatServer::OnCloseConnection()     { }


void ChatServer::ConnectionThreadFunction()
{
    while(!m_bGoingExit)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        SendStringToAllClients("SERVER IS WORKING\n");
		s_pCallbackInstance = this;
		m_pInterface->RunCallbacks();
    }
	for ( auto it: m_mapClients )
	{
		// Send them one more goodbye message.  Note that we also have the
		// connection close reason as a place to send final data.  However,
		// that's usually best left for more diagnostic/debug text not actual
		// protocol strings.
		SendStringToClient( it.first, "Server is shutting down.  Goodbye." );

		// Close the connection.  We use "linger mode" to ask SteamNetworkingSockets
		// to flush this out and close gracefully.
		m_pInterface->CloseConnection( it.first, 0, "Server Shutdown", true );
	}
	m_mapClients.clear();

	m_pInterface->CloseListenSocket( m_hListenSock );
	m_hListenSock = k_HSteamListenSocket_Invalid;

	m_pInterface->DestroyPollGroup( m_hPollGroup );
	m_hPollGroup = k_HSteamNetPollGroup_Invalid;
}

void ChatServer::SendStringToClient( HSteamNetConnection conn, const char *str )
{
	m_pInterface->SendMessageToConnection( conn, str, (uint32)strlen(str), k_nSteamNetworkingSend_Reliable, nullptr );
}

void ChatServer::SendStringToAllClients( const char *str, HSteamNetConnection except  )
{
	for ( auto &c: m_mapClients )
	{
		if ( c.first != except )
			SendStringToClient( c.first, str );
	}
}

void ChatServer::OnSteamNetConnectionStatusChanged( SteamNetConnectionStatusChangedCallback_t *pInfo )
{
	//char temp[1024];
	// What's the state of the connection?
	switch ( pInfo->m_info.m_eState )
	{
		case k_ESteamNetworkingConnectionState_None:
			// NOTE: We will get callbacks here when we destroy connections.  You can ignore these.
			break;
		case k_ESteamNetworkingConnectionState_ClosedByPeer:
		case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
		{
			// Ignore if they were not previously connected.  (If they disconnected
			// before we accepted the connection.)
			if ( pInfo->m_eOldState == k_ESteamNetworkingConnectionState_Connected )
			{
				// Locate the client.  Note that it should have been found, because this
				// is the only codepath where we remove clients (except on shutdown),
				// and connection change callbacks are dispatched in queue order.
				auto itClient = m_mapClients.find( pInfo->m_hConn );
				CHAT_ASSERT(itClient != m_mapClients.end(), "Empty m_mapClients");
				// Select appropriate log messages
				const char * pszDebugLogAction;
				if ( pInfo->m_info.m_eState == k_ESteamNetworkingConnectionState_ProblemDetectedLocally )
				{
					pszDebugLogAction = "problem detected locally";
					//sprintf( temp, "Alas, %s hath fallen into shadow.  (%s)", itClient->second.m_sNick.c_str(), pInfo->m_info.m_szEndDebug );
				}
				else
				{
					// Note that here we could check the reason code to see if
					// it was a "usual" connection or an "unusual" one.
					pszDebugLogAction = "closed by peer";
					//sprintf( temp, " hath departed", itClient->second.m_sNick.c_str() );
				}
				(void) pszDebugLogAction;
				// Spew something to our own log.  Note that because we put their nick
				// as the connection description, it will show up, along with their
				// transport-specific data (e.g. their IP address)
				//Printf( "Connection %s %s, reason %d: %s\n",
				//	pInfo->m_info.m_szConnectionDescription,
				//	pszDebugLogAction,
				//	pInfo->m_info.m_eEndReason,
				//	pInfo->m_info.m_szEndDebug
				//);
				m_mapClients.erase( itClient );
				// Send a message so everybody else knows what happened
				//SendStringToAllClients( temp );
			}
			else
			{
				CHAT_ASSERT( pInfo->m_eOldState == k_ESteamNetworkingConnectionState_Connecting , "");
			}
			// Clean up the connection.  This is important!
			// The connection is "closed" in the network sense, but
			// it has not been destroyed.  We must close it on our end, too
			// to finish up.  The reason information do not matter in this case,
			// and we cannot linger because it's already closed on the other end,
			// so we just pass 0's.
			m_pInterface->CloseConnection( pInfo->m_hConn, 0, nullptr, false );
			break;
		}
		case k_ESteamNetworkingConnectionState_Connecting:
		{
			// This must be a new connection
			CHAT_ASSERT( m_mapClients.find( pInfo->m_hConn ) == m_mapClients.end(), "m_mapClients should not have this con yet" );
			//Printf( "Connection request from %s", pInfo->m_info.m_szConnectionDescription );
			// A client is attempting to connect
			// Try to accept the connection.
			if ( m_pInterface->AcceptConnection( pInfo->m_hConn ) != k_EResultOK )
			{
				// This could fail.  If the remote host tried to connect, but then
				// disconnected, the connection may already be half closed.  Just
				// destroy whatever we have on our side.
				m_pInterface->CloseConnection( pInfo->m_hConn, 0, nullptr, false );
				//Printf( "Can't accept connection.  (It was already closed?)" );
				break;
			}
			// Assign the poll group
			if ( !m_pInterface->SetConnectionPollGroup( pInfo->m_hConn, m_hPollGroup ) )
			{
				m_pInterface->CloseConnection( pInfo->m_hConn, 0, nullptr, false );
				//Printf( "Failed to set poll group?" );
				break;
			}
			// Generate a random nick.  A random temporary nick
			// is really dumb and not how you would write a real chat server.
			// You would want them to have some sort of signon message,
			// and you would keep their client in a state of limbo (connected,
			// but not logged on) until them.  I'm trying to keep this example
			// code really simple.
			//char nick[ 64 ];
			//sprintf( nick, "BraveWarrior%d", 10000 + ( rand() % 100000 ) );
			//// Send them a welcome message
			//sprintf( temp, "Welcome, stranger.  Thou art known to us for now as '%s'; upon thine command '/nick' we shall know thee otherwise.", nick ); 
			//SendStringToClient( pInfo->m_hConn, temp ); 
			//// Also send them a list of everybody who is already connected
			//if ( m_mapClients.empty() )
			//{
			//	SendStringToClient( pInfo->m_hConn, "Thou art utterly alone." ); 
			//}
			//else
			//{
			//	sprintf( temp, "%d companions greet you:", (int)m_mapClients.size() ); 
			//	for ( auto &c: m_mapClients )
			//		SendStringToClient( pInfo->m_hConn, c.second.m_sNick.c_str() ); 
			//}
			//// Let everybody else know who they are for now
			//sprintf( temp, "Hark!  A stranger hath joined this merry host.  For now we shall call them '%s'", nick ); 
			//SendStringToAllClients( temp, pInfo->m_hConn ); 
			//// Add them to the client list, using std::map wacky syntax
			m_mapClients[ pInfo->m_hConn ];
			m_mapClients[ pInfo->m_hConn ].GetNickname() = _T("Blank");
			//SetClientNick( pInfo->m_hConn, nick );
			break;
		}
		case k_ESteamNetworkingConnectionState_Connected:
			// We will get a callback immediately after accepting the connection.
			// Since we are the server, we can ignore this, it's not news to us.
			break;
		default:
			// Silences -Wswitch
			break;
	}
}



} /*END OF NAMESPACES*/