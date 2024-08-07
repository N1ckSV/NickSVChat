
#include "NickSV/Chat/Types.h"
#include "NickSV/Chat/Utils.h"
#include "NickSV/Chat/Parsers/BStringParser.h"

#include "NickSV/Chat/Requests/Request.h"
#include "NickSV/Chat/Requests/MessageRequest.h"
#include "NickSV/Chat/Requests/ClientInfoRequest.h"

#include "NickSV/Chat/ChatClient.h"
#include "NickSV/Chat/ChatServer.h"


#include "NickSV/Tools/ValueLock.h"


#include <string>
#include <thread>
#include <chrono>
#include <iostream>
#include <random>
#include <iomanip>
#include <cstdio>


#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#else // Supposed Linux here
#endif


#undef GetMessage

using namespace NickSV;
using namespace NickSV::Chat;

struct ExampleChatServerException : std::runtime_error
{
    ExampleChatServerException() : std::runtime_error(
        "ExampleServer got bad request") {}
};


struct ExampleChatClientException : std::runtime_error
{
    ExampleChatClientException() : std::runtime_error(
        "ExampleClient cannot send request") {}
};

static bool SetConsoleMode()
{
#ifdef _WIN32
	std::flush(std::wcout);
	return ((_setmode(_fileno(stdin ), _O_U16TEXT) != -1) &&
	   		(_setmode(_fileno(stdout), _O_U16TEXT) != -1));
#else // Supposed Linux here
#endif
}


class ExampleServer : public ChatServer
{
	using ChatServer::OnHandleRequest;

	void OnRun(const NickSV::Chat::ChatIPAddr &, 
				NickSV::Chat::EResult,
	 			NickSV::Chat::ChatErrorMsg&) override
	{
		std::wcout << "\nServer is started.\n" << std::endl;
	}
    void OnHandleRequest(const MessageRequest& rcReq, RequestInfo,  
	NickSV::Chat::EResult outsideResult, TaskInfo) override
    {
        if(outsideResult != NickSV::Chat::EResult::Success)
            return;

        std::wcout 
			<< std::endl 
			<< rcReq.GetMessage().GetSenderID() 
			<< TEXT(": ", CHAT_CHAR) 
			<< rcReq.GetMessage().GetText() 
			<< std::endl;
		if(rcReq.GetMessage().GetText() == TEXT("/exit", CHAT_CHAR))
		{	
			std::wcout <<  TEXT("Closing Server...", CHAT_CHAR) << std::endl;
			std::this_thread::sleep_for(std::chrono::seconds(3));
			CloseSocket();
		};
	}
	void OnAcceptClient(ConnectionInfo&, ClientInfo& rClientInfo, NickSV::Chat::EResult, TaskInfo) override
    {
		std::wcout << "\nNew CLient connected with id: " << rClientInfo.GetUserID() << std::endl << std::endl;
    }

	void OnHandleRequest(const Request& req, RequestInfo reqInfo, NickSV::Chat::EResult result)
    {
		std::wcout << "\nGot "<< req.GetType() <<" from client with id " << reqInfo.userID << std::endl << std::endl;
    }

	void OnHandleRequest(const ClientInfoRequest& rcRer, RequestInfo, 
                                 ClientInfo& rClientInfo, 
                                 NickSV::Chat::EResult outsideResult,
                                 TaskInfo) override
    {
		std::wcout << "\nClient with id " << rClientInfo.GetUserID() << " now is " << rClientInfo.GetState() << std::endl << std::endl;
    }
	
	void OnConnectionStatusChanged(ConnectionInfo* pInfo, NickSV::Chat::EResult res) override
	{
		std::wcout << "\nStatus changed from " << pInfo->m_eOldState
				   << " to " << pInfo->m_info.m_eState
				   << " and result is " << res << std::endl;
		for (auto& client: GetClients()) 
		{
			std::wcout << "Client " << client.GetUserID() << " - state " << client.GetState() << std::endl; 
		}
	}

	void OnBadIncomingRequest(std::string strReq, ClientInfo&, NickSV::Chat::EResult res) override
    {
		union 
    	{
    	    CHAT_CHAR* charType;
    	    char* Char;
    	} pStr;
		std::basic_string<CHAT_CHAR> bstring;
    	bstring.resize(size_t(float(strReq.size())/sizeof(CHAT_CHAR)) + ((strReq.size() % sizeof(CHAT_CHAR)) > 0));
    	pStr.charType = bstring.data();
    	std::copy(strReq.begin(), strReq.end(), pStr.Char);
		std::wcout 	<< TEXT("\nBad request received with Result: ", CHAT_CHAR)
		<< int(res) << TEXT(". Raw request text: \"", CHAT_CHAR)
		<< bstring 	<< TEXT("\"", CHAT_CHAR) << std::endl;
        throw ExampleChatServerException();
    }
};

class ExampleClient : public ChatClient
{
	using ChatClient::OnHandleRequest;

	void OnHandleRequest(const MessageRequest& rcReq, RequestInfo,  
	NickSV::Chat::EResult outsideResult) override
	{
		if(outsideResult != NickSV::Chat::EResult::Success)
			return;

        std::wcout << std::endl <<
		 rcReq.GetMessage().GetSenderID() << TEXT(": ", CHAT_CHAR) << rcReq.GetMessage().GetText() << std::endl;
	}

	void OnHandleRequest(const ClientInfoRequest& rcRer, RequestInfo,
                                 NickSV::Chat::EResult res, TaskInfo) override
    {
		std::wcout 
		 << "\nGot ClientInfoRequest and now we " << rcRer.GetClientInfo().GetState()
		 << " with ID " << rcRer.GetClientInfo().GetUserID()
		 << " and result is " << res << std::endl << std::endl;
    }
	
	void OnConnectionStatusChanged(ConnectionInfo *pInfo, NickSV::Chat::EResult res) override
	{
		//std::wcout 
		// << "\nStatus changed from " << pInfo->m_eOldState
		// << " to " << pInfo->m_info.m_eState
		// << " and result is " << res << std::endl << std::endl;
	}

	void OnQueueRequest(const Request&, RequestInfo, NickSV::Chat::EResult res) override
	{
		if(!ResultIsOneOf(res, NickSV::Chat::EResult::Success, NickSV::Chat::EResult::ClosedSocket))
		{
        	throw ExampleChatClientException();
		}

	}
	void OnFatalError(const std::string& errorMsg)
	{
		std::wcout 
		 << "\nFatal error. " << Tools::basic_string_convert<CHAT_CHAR>(errorMsg.c_str()) << std::endl << std::endl;
	}
	
    NickSV::Chat::EResult OnPreConnect(ChatIPAddr &serverAddr, std::chrono::milliseconds &timeout, unsigned int &maxAttempts) override
	{
		std::wcout 
		<< "\nTrying to connect." << std::endl << std::endl;
		return NickSV::Chat::EResult::Success;
	}

    void OnConnect(const ChatIPAddr &serverAddr, NickSV::Chat::EResult res, std::chrono::milliseconds reconnectTime, unsigned int countRetries) override
	{
		std::wcout 
		<< "\nConnect result: " << res << std::endl << std::endl;
	}
    void OnConnectAttemptFailed(ChatIPAddr &serverAddr, std::chrono::milliseconds reconnectTime, unsigned int countRetries, unsigned int maxAttempts) override
	{
		std::wcout 
		<< "\nIP: " << 
		Tools::basic_string_convert<CHAT_CHAR>(serverAddr.ToString().c_str()) << ". Try "<< countRetries << '/' << maxAttempts << ". Time: " << reconnectTime.count() << ". Reconnecting... " << std::endl << std::endl;
	}
};

int main(int argc, const char *argv[])
{
	if(!SetConsoleMode())
	{
		std::wcout << "Cannot set console mode" << std::endl;
		return -1;
	}
	bool bServer = false;
	bool bClient = false;
	ChatIPAddr addrServer; addrServer.Clear();
    addrServer.ParseString("192.168.0.212"); //DEV'S pc local ip
    addrServer.m_port = DEFAULT_PORT;

	for (int i = 1 ; i < argc ; ++i)
	{
		if (!bClient && !bServer)
		{
			if (!strcmp(argv[i], "client"))
			{
				bClient = true;
				continue;
			}
			if (!strcmp(argv[i], "server"))
			{
				bServer = true;
				continue;
			}
		}
	}
	if (bClient == bServer || (bClient && addrServer.IsIPv6AllZeros()))
        return 1;

	if (bClient)
	{
		ExampleClient client;
		client.Run(addrServer);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::basic_string<CHAT_CHAR> cmd;
        while(client.IsRunning())
        {
			std::wcout << "Type message (or /exit): ";
			std::getline(std::wcin, cmd);
			std::wcout << "You: " << cmd << std::endl;
            MessageRequest mReq {Message(cmd)};
			NickSV::Chat::EResult result;
            auto taskInfo = client.QueueRequest(mReq);
			try
			{
				result = taskInfo.GetFutureResult();
			}
			catch(const std::future_error& e)
			{
				std::wcout << NickSV::Tools::basic_string_convert<CHAT_CHAR>(e.what()) << e.code() << std::endl;
        		std::this_thread::sleep_for(std::chrono::milliseconds(100));
				client.CloseSocket();
			}
			if(result != NickSV::Chat::EResult::Success)
				std::wcout << "Message was not sent, error code: " << result << std::endl;

        	std::this_thread::sleep_for(std::chrono::milliseconds(100));
			if(cmd == TEXT("/exit", CHAT_CHAR)){
				client.CloseSocket();
				break;}
        }
		client.Wait();
	}
	else
	{
		ExampleServer server;
		server.Run(addrServer);
		server.Wait();
		std::wcout << std::endl << TEXT("Server is shut down", CHAT_CHAR);
        std::this_thread::sleep_for(std::chrono::seconds(10));
	}
    return 0;
}