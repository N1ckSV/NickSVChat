

#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#else // Supposed Linux here
#endif


#include <string>
#include <thread>
#include <chrono>
#include <iostream>
#include <random>
#include <iomanip>
#include <cstdio>
#include <locale>


#include "NickSV/Tools/ValueLock.h"

#include "NickSV/Chat/ChatClient.h"
#include "NickSV/Chat/ChatServer.h"
#include "NickSV/Chat/Message.h"

#include "UserName.pb.h"


#ifdef _WIN32
	#define sout std::wcout
	#define scin std::wcin
	#define _T(x) L##x
	#define VAR_CHAR wchar_t
	#include <codecvt>
	#define UTF8_TO_WCHAR(x) \
	std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(x)
	#define WCHAR_TO_UTF8(x) \
	std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(x)

	
	//#include <sstream>
	//#include <Windows.h>
	//#define sout std::cout
	//#define scin std::cin
	//#define _T(x) u8##x
	//#define VAR_CHAR char
	//#define UTF8_TO_WCHAR(x) x
	//#define WCHAR_TO_UTF8(x) x
//
	//class MBuf: public std::stringbuf {
	//public:
	//    int sync() {
	//        fputs( str().c_str(), stdout );
	//        str( "" );
	//        return 0;
	//    }
	//};
#else
	#define sout std::cout
	#define scin std::cin
	#define _T(x) u8##x
	#define VAR_CHAR char
	#define UTF8_TO_WCHAR(x) x
	#define WCHAR_TO_UTF8(x) x
#endif

#define STR(x) Tools::basic_string_convert<VAR_CHAR>(x)



/**
 * Sometimes WSL can type non-utf-8 characters (e.g. typing Backspace)
 */
bool IsValidUtf8String(const std::string& str)
{
    size_t i = 0;
    while (i < str.size()) 
	{
        unsigned char c = str[i];
        size_t num_bytes = 0;

        if (c <= 0x7F)
            num_bytes = 1;
        else if ((c & 0xE0) == 0xC0)
            num_bytes = 2;
        else if ((c & 0xF0) == 0xE0)
            num_bytes = 3;
        else if ((c & 0xF8) == 0xF0)
            num_bytes = 4;
        else
            return false;
			
        if (i + num_bytes > str.size())
            return false;

        for (size_t j = 1; j < num_bytes; ++j) 
		{
            if ((str[i + j] & 0xC0) != 0x80)
                return false;
        }
        i += num_bytes;
    }

    return true;
}






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
	//system("chcp 65001 >nul");
	std::flush(sout);
	return ((_setmode(_fileno(stdin ), _O_U16TEXT) != -1) &&
	   		(_setmode(_fileno(stdout), _O_U16TEXT) != -1));
#else
    std::locale::global(std::locale("en_US.UTF-8"));
	return true;
#endif
}


class ExampleServer : public ChatServer
{
	using ChatServer::OnHandleRequest;

	void OnRun(const NickSV::Chat::ChatIPAddr &, 
				NickSV::Chat::EResult,
	 			NickSV::Chat::ChatErrorMsg&) override
	{
		sout << _T("\nServer is started.\n") << std::endl;
	}
    void OnHandleRequest(const MessageRequest& rcReq, RequestInfo,  
	NickSV::Chat::EResult outsideResult, TaskInfo) override
    {
        if(outsideResult != NickSV::Chat::EResult::Success)
            return;

		Message message;
		rcReq.UnpackMessageTo(&message);
		UserName name;
		CHAT_ASSERT(message.HasAdditionalData(), "Message has no additional data");
		message.UnpackAdditionalDataTo(&name);

        sout
			<< std::endl 
			<< UTF8_TO_WCHAR(name.text())
			<< _T(": " )
			<< UTF8_TO_WCHAR(message.Text())
			<< std::endl;
		if(message.Text() == u8"!exit") // u8 fow Windows also, dont need fix
		{	
			sout <<  _T("Closing Server..." )<< std::endl;
			std::this_thread::sleep_for(std::chrono::seconds(3));
			CloseSocket();
		};
	}
	void OnAcceptClient(const ConnectionInfo&, const RequestInfo& reqInfo, ClientInfo& rClientInfo, NickSV::Chat::EResult res, TaskInfo tInfo) override
    {
		if(res != NickSV::Chat::EResult::Success)
		{
			sout << _T("\nNew Client not accepted ") << std::endl << std::endl;
			return;
		}
		sout << _T("\nNew Client accepted with id: ") << rClientInfo.UserID() << std::endl << std::endl;
    }
	void OnQueueRequest(const Request&, RequestInfo, NickSV::Chat::EResult res) override
	{
		if(!ResultIsOneOf(res, NickSV::Chat::EResult::Success))
		{
        	throw ExampleChatServerException();
		}
	}

	void OnHandleRequest(const Request& req, RequestInfo reqInfo, NickSV::Chat::EResult result)
    {
		sout << _T("\nGot ")<< req.Type() << _T(" from client with id ") << reqInfo.userID << std::endl << std::endl;
    }

	void OnHandleRequest(const ClientInfoRequest& rcRer, RequestInfo, 
                                 ClientInfo& rClientInfo, 
                                 NickSV::Chat::EResult outsideResult,
                                 TaskInfo) override
    {
		sout << _T("\nClient with id ") << rClientInfo.UserID() << _T(" now is ") << rClientInfo.State() << std::endl << std::endl;
    }
	
	void OnConnectionStatusChanged(ConnectionInfo* pInfo, NickSV::Chat::EResult res) override
	{
		sout << _T("\nStatus changed from ") << pInfo->m_eOldState
				   << _T(" to ") << pInfo->m_info.m_eState
				   << _T(" and result is ") << res << std::endl;
		for (auto& client: GetClients()) 
		{
			sout << _T("Client ") << client.UserID() << _T(" - state ") << client.State() << std::endl; 
		}
	}

	void OnBadIncomingRequest(std::string strReq, ClientInfo&, NickSV::Chat::EResult res) override
    {
		sout 	
			<< _T("\nBad request received with Result: ") << int(res) 
			<< _T(". Raw request text: \"") << STR(strReq.c_str())
			<< _T("\"") << std::endl;
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
			throw ExampleChatClientException();

		Message message;
		rcReq.UnpackMessageTo(&message);
		UserName name;
		CHAT_ASSERT(message.HasAdditionalData(), "Message has no additional data");
		message.UnpackAdditionalDataTo(&name);

        sout << std::endl 
			 << UTF8_TO_WCHAR(name.text())
			 << _T(": ")
			 << UTF8_TO_WCHAR(message.Text()) << std::endl;
	}

	void OnHandleRequest(const ClientInfoRequest& rcReq, RequestInfo,
                                 NickSV::Chat::EResult res, TaskInfo) override
    {
		ClientInfo clientInfo;
		if(!rcReq.UnpackClientInfoTo(&clientInfo))
			throw ExampleChatClientException();

		sout
		 << _T("\nGot ClientInfoRequest and now we ") << clientInfo.State()
		 << _T(" with ID ") << clientInfo.UserID()
		 << _T(" and result is ") << res << std::endl << std::endl;
    }

	void OnQueueRequest(const Request&, RequestInfo, NickSV::Chat::EResult res) override
	{
		if(!ResultIsOneOf(res, NickSV::Chat::EResult::Success, NickSV::Chat::EResult::ClosedSocket))
        	throw ExampleChatClientException();

	}
	void OnFatalError(const std::string& errorMsg)
	{
		sout
		 << _T("\nFatal error. ") << STR(errorMsg.c_str()) << std::endl << std::endl;
	}
	
    NickSV::Chat::EResult OnPreConnect(ChatIPAddr &serverAddr, std::chrono::milliseconds &timeout, unsigned int &maxAttempts) override
	{
		sout
		<< _T("\nTrying to connect.") << std::endl << std::endl;
		return NickSV::Chat::EResult::Success;
	}

    void OnConnect(const ChatIPAddr &serverAddr, NickSV::Chat::EResult res, std::chrono::milliseconds reconnectTime, unsigned int countRetries) override
	{
		sout
		<< _T("\nConnect result: ") << res << std::endl << std::endl;
	}
    void OnConnectAttemptFailed(ChatIPAddr &serverAddr, std::chrono::milliseconds reconnectTime, unsigned int countRetries, unsigned int maxAttempts) override
	{
		sout 
			<< _T("\nIP: ") << STR(serverAddr.ToString().c_str()) 
			<< _T(". Try ")<< countRetries 
			<< _T('/') << maxAttempts 
			<< _T(". Time: ") << reconnectTime.count() 
			<< _T(". Reconnecting... ") << std::endl << std::endl;
	}
};

int main(int argc, const char *argv[])
{
	//cppcheck-suppress knownConditionTrueFalse
	if(!SetConsoleMode())
	{
		sout << "Cannot set console mode" << std::endl;
		return -1;
	}

	//Check if UTF-8 chars is looking valid
	sout << _T(R"(
UTF-8 test:
		  ∮ E⋅da = Q,  n → ∞, ∑ f(i) = ∏ g(i), ∀x∈ℝ: ⌈x⌉ = −⌊−x⌋, α ∧ ¬β = ¬(¬α ∨ β),
		  ℕ ⊆ ℕ₀ ⊂ ℤ ⊂ ℚ ⊂ ℝ ⊂ ℂ, ⊥ < a ≠ b ≡ c ≤ d ≪ ⊤ ⇒ (A ⇔ B),
		  2H₂ + O₂ ⇌ 2H₂O, R = 4.7 kΩ, ⌀ 200 mm
		  ði ıntəˈnæʃənəl fəˈnɛtık əsoʊsiˈeıʃn
		  Y [ˈʏpsilɔn], Yen [jɛn], Yoga [ˈjoːgɑ]
UTF-8 test end.)")
	<< std::endl;

	bool bServer = false;
	bool bClient = false;

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
	if (bClient == bServer)
        return 1;

	if (bClient)
	{
		std::basic_string<VAR_CHAR> name_text;
		bool validString = false;
		std::string utf8str;
		while(true)
		{
			sout << _T("Type your nickname: ") << std::flush;
			std::getline(scin, name_text);
			utf8str = WCHAR_TO_UTF8(name_text);
			if(IsValidUtf8String(utf8str))
				break;

			sout << _T("Non-UTF8 characters found. Please type valid nickname.") << std::endl;
		}
		UserName name;
		name.set_text(utf8str);
		ExampleClient client;
		ChatIPAddr addrServer; addrServer.Clear();
    	addrServer.ParseString("192.168.0.212"); //DEV'S pc local ip
    	addrServer.m_port = DEFAULT_PORT;
		client.Run(addrServer);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::basic_string<VAR_CHAR> cmd;
        while(client.IsRunning())
        {
			sout << _T("Type message (or !exit): ") << std::flush;
			std::getline(scin, cmd);
			utf8str = WCHAR_TO_UTF8(cmd);
			if(!IsValidUtf8String(utf8str))
			{
				sout << _T("Non-UTF8 characters found. Please type valid message.") << std::endl;
				continue;
			}
			sout << _T("You: ") << cmd << std::endl;
            MessageRequest mReq;
			Message message;
			message.PackAdditionalDataFrom(name);
			message.SetText(utf8str);
			mReq.PackMessageFrom(message);
			NickSV::Chat::EResult result = NickSV::Chat::EResult::Success;
            auto taskInfo = client.QueueRequest(mReq);
			try
			{
				result = taskInfo.GetFutureResult();
			}
			catch(const std::future_error& e)
			{
				sout << e.what() << e.code() << std::endl;
        		std::this_thread::sleep_for(std::chrono::milliseconds(100));
				client.CloseSocket();
			}
			if(result != NickSV::Chat::EResult::Success)
				sout << _T("Message was not sent, error code: ") << result << std::endl;

        	std::this_thread::sleep_for(std::chrono::milliseconds(100));
			if(cmd == _T("!exit")){
				client.CloseSocket();
				break;}
        }
		client.Wait();
	}
	else
	{
		ExampleServer server;
		ChatIPAddr addrServer; addrServer.Clear();
    	addrServer.m_port = DEFAULT_PORT;
		server.Run(addrServer);
		server.Wait();
		sout << std::endl << _T("Server is shut down");
        std::this_thread::sleep_for(std::chrono::seconds(10));
	}
    return 0;
}