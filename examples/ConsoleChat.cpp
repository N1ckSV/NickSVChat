
#include "NickSV/Chat/Types.h"
#include "NickSV/Chat/Utils.h"

#include "NickSV/Chat/Requests/Request.h"
#include "NickSV/Chat/Requests/MessageRequest.h"
#include "NickSV/Chat/Requests/ClientInfoRequest.h"

#include "NickSV/Chat/ChatClient.h"
#include "NickSV/Chat/ChatServer.h"

#include <string>
#include <thread>
#include <chrono>
#include <iostream>

using namespace NickSV::Chat;

class ExampleServer : public ChatServer
{
    void OnHandleRequest(const Request* pcRer, RequestInfo reqInfo,  NickSV::Chat::EResult outsideResult) override
    {
        if(outsideResult != NickSV::Chat::EResult::Success)
            return;

        if(pcRer->GetType() == ERequestType::Message)
        {
            auto pcmReq = static_cast<const MessageRequest*>(pcRer);
            std::wcout << pcmReq->GetMessage()->GetText() << std::endl;
        }
    }
};

int main(int argc, const char *argv[])
{
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
			std::wcout << L"Type message (or /exit): "; 
            std::wcin >> cmd;
			if(cmd == L"/exit"){
				client.CloseSocket();
				break;}
            MessageRequest mReq;
            *mReq.GetMessage() = Message(cmd);
            client.QueueRequest(&mReq, {0,0});
        }
		client.Wait();
	}
	else
	{
		ExampleServer server;
		server.Run(addrServer);
        std::this_thread::sleep_for(std::chrono::seconds(100));
		server.CloseSocket();
		server.Wait();
	}
    return 0;
}