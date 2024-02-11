
#include "NickSV/Chat/ChatClient.h"
#include "NickSV/Chat/ClientInfo.h"
#include "NickSV/Chat/Defines.h"
#include <iostream>

namespace NickSV::Chat {

void ChatClient::Run(const SteamNetworkingIPAddr &serverAddr )
{
    /*Description*/

    
    /*
      ERROR: Chat's connection is already running or m_pMainThread is'n null.
      Call CloseConnection() before calling Run() second time.
      Asserts that m_pMainThread in null, memory leak instead.
      IsRunning() == false should always indicate that m_pMainThread is null.
    */
    CHAT_ASSERT(!IsRunning() && !m_pMainThread, "ERROR: Connection is already running.");

    this->OnPreStartConnection();
    ///////////////////////////////////


    m_pInterface = SteamNetworkingSockets();
	SteamNetworkingConfigValue_t opt;
	opt.SetPtr( k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged, (void*)SteamNetConnectionStatusChangedCallback );
    m_hConnection = m_pInterface->ConnectByIPAddress( serverAddr, 1,&opt);    

    if ( m_hConnection == k_HSteamNetConnection_Invalid )
        return;

    m_bGoingExit = false; //SHOULD BE FIRST BEFORE m_pMainThread = new ...
    m_pMainThread = new std::thread(&ChatClient::MainThreadFunction, this);
    //TODO: exit/kill/nuke/terminate

    ///////////////////////////////////
    this->OnStartConnection();
}

void ChatClient::SetInfo(ClientInfo<>* pChatClientInfo)
{
	
}

void ChatClient::CloseConnection()
{
    /*Description*/

    //ERROR: m_pMainThread is null.
    CHAT_ASSERT(m_pMainThread, "ERROR: Connection thread is missing (m_pMainThread == null).");

    this->OnPreCloseConnection();
    ///////////////////////////////////


    //TODO
    //FIXME FIXME FIXME FIXME FIXME FIXME
    m_bGoingExit = true;
    m_pMainThread->join();
    delete m_pMainThread;
    m_pMainThread = nullptr;


    ///////////////////////////////////
    this->OnCloseConnection();
}
bool inline ChatClient::IsRunning()
{
    return !m_bGoingExit;
}
ChatClient::ChatClient()
{
    m_bGoingExit = true;
    m_pMainThread = nullptr;
	m_pClientInfo = new ClientInfo<>();
	/////////
}
ChatClient::~ChatClient()
{
    //TODO
    //FIXME FIXME FIXME FIXME FIXME FIXME 
	delete m_pClientInfo;
    delete m_pMainThread;
}

void ChatClient::OnPreStartConnection()  { };
void ChatClient::OnStartConnection()     { };
void ChatClient::OnPreCloseConnection()  { };
void ChatClient::OnCloseConnection()     { };

void ChatClient::MainThreadFunction()
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