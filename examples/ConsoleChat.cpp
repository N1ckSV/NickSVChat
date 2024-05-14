
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
    void OnHandleMessageRequest(const MessageRequest* pcRer, RequestInfo reqInfo,  NickSV::Chat::EResult outsideResult) override
    {
        if(outsideResult != NickSV::Chat::EResult::Success)
            return;

        std::wcout << pcRer->GetMessage()->GetText() << std::endl;
		std::wcout <<  Tools::basic_string_cast<CHAT_CHAR>("Converted wchar_t: ") << *reinterpret_cast<const uint16_t*>(pcRer->GetMessage()->GetText().c_str()) << std::endl;
		if(pcRer->GetMessage()->GetText() == Tools::basic_string_cast<CHAT_CHAR>("/exit"))
			this->CloseSocket();
	}
	void OnAcceptClient(ConnectionInfo* pInfo, ClientInfo* pClientInfo, NickSV::Chat::EResult res) override
    {
        pClientInfo->GetState() = EState::Active;
		std::wcout << "\nNew CLient connected with id: " << pClientInfo->GetUserID() << std::endl;
    }
	void OnBadIncomingRequest(std::string strReq, NotNull<ClientInfo*> pClientInfo, NickSV::Chat::EResult res) override
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
		std::wcout 	<< Tools::basic_string_cast<CHAT_CHAR>("\nBad request received with Result: ")
		<< int(res) << Tools::basic_string_cast<CHAT_CHAR>(". Raw request text: \"")
		<< bstring 	<< Tools::basic_string_cast<CHAT_CHAR>("\"") << std::endl;
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
			std::wcout << Tools::basic_string_cast<CHAT_CHAR>("Type message (or /exit): "); 
            std::wcin >> cmd;
			std::wcout << Tools::basic_string_cast<CHAT_CHAR>("You typed: ") << cmd << std::endl;
			std::wcout << Tools::basic_string_cast<CHAT_CHAR>("Converted wchar_t: ") << *reinterpret_cast<const uint16_t*>(cmd.c_str()) << std::endl;
            MessageRequest mReq;
            *mReq.GetMessage() = Message(cmd);
            client.QueueRequest(&mReq, {0,0});
        	std::this_thread::sleep_for(std::chrono::milliseconds(100));
			if(cmd == Tools::basic_string_cast<CHAT_CHAR>("/exit")){
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
		std::wcout << std::endl << Tools::basic_string_cast<CHAT_CHAR>("Server is shut down");
        std::this_thread::sleep_for(std::chrono::seconds(10));
	}
    return 0;
}