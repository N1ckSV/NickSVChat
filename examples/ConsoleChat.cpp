
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

bool SetConsoleMode()
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
    void OnHandleMessageRequest(const MessageRequest& rcReq, RequestInfo,  NickSV::Chat::EResult outsideResult) override
    {
        if(outsideResult != NickSV::Chat::EResult::Success)
            return;

        std::wcout << rcReq.GetMessage()->GetText() << std::endl;
		std::wcout <<  TEXT("Converted wchar_t: ", CHAT_CHAR) << *reinterpret_cast<const uint16_t*>(rcReq.GetMessage()->GetText().c_str()) << std::endl;
		if(rcReq.GetMessage()->GetText() == TEXT("/exit", CHAT_CHAR))
			this->CloseSocket();
	}
	void OnAcceptClient(ConnectionInfo&, ClientInfo* pClientInfo, NickSV::Chat::EResult) override
    {
        pClientInfo->GetState() = EState::Active;
		std::wcout << "\nNew CLient connected with id: " << pClientInfo->GetUserID() << std::endl;
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
		ChatClient client;
		client.Run(addrServer);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::basic_string<CHAT_CHAR> cmd;
        while(client.IsRunning())
        {
			std::wcout << TEXT("Type message (or /exit): ", CHAT_CHAR);
			std::getline(std::wcin, cmd);
			std::wcout << TEXT("You typed: ", CHAT_CHAR) << cmd << std::endl;
			std::wcout << TEXT("Converted wchar_t: ", CHAT_CHAR) << *reinterpret_cast<const uint16_t*>(cmd.c_str()) << std::endl;
            MessageRequest mReq;
            *mReq.GetMessage() = Message(cmd);
            client.QueueRequest(mReq, {0,0});
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