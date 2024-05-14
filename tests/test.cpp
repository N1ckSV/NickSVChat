#include "NickSV/Chat/Utils.h"

#include "NickSV/Chat/ClientInfo.h"
#include "NickSV/Chat/BasicMessage.h"

#include "NickSV/Chat/Requests/Request.h"
#include "NickSV/Chat/Requests/MessageRequest.h"
#include "NickSV/Chat/Requests/ClientInfoRequest.h"

#include "NickSV/Chat/Serializers/ClientInfoSerializer.h"
#include "NickSV/Chat/Serializers/BMessageSerializer.h"
#include "NickSV/Chat/Serializers/BStringSerializer.h"
#include "NickSV/Chat/Serializers/ClientInfoRequestSerializer.h"
#include "NickSV/Chat/Serializers/MessageRequestSerializer.h"

#include "NickSV/Chat/Parsers/ClientInfoParser.h"
#include "NickSV/Chat/Parsers/BMessageParser.h"
#include "NickSV/Chat/Parsers/BStringParser.h"
#include "NickSV/Chat/Parsers/RequestParser.h"
#include "NickSV/Chat/Parsers/ClientInfoRequestParser.h"
#include "NickSV/Chat/Parsers/MessageRequestParser.h"

#include "NickSV/Chat/ChatClient.h"
#include "NickSV/Chat/ChatServer.h"

#include "NickSV/Tools/ValueLock.h"
#include "NickSV/Tools/Testing.h"

#include <type_traits>
#include <thread>

#ifdef _WIN32
#include <Windows.h>
#else
#include <pthread.h>
#endif


using namespace NickSV;
using namespace NickSV::Chat;

size_t client_info_test()
{
    ClientInfo info1, info2;
    info1.GetUserID() = 12345;
    info2.GetUserID() = 12345;
    TEST_CHECK_STAGE(info1 == info2);

    info2.GetUserID() = 10101;
    TEST_CHECK_STAGE(info1 != info2);

    info1.GetUserID() = 12345;
    TEST_CHECK_STAGE(info1 != info2);

    return TEST_SUCCESS;
}


size_t client_info_serializers_and_parsers_test()
{
    UserID_t id = 2;
    const auto ClientInfo1 = ClientInfo(id);
    ClientInfo ClientInfo2;
    auto str = ClientInfo1.GetSerializer()->ToString();
    ClientInfo2 = MakeFromString<ClientInfo>(str);
    TEST_CHECK_STAGE(ClientInfo2 == ClientInfo1);

    str.resize(str.size()-1);
    ClientInfo2 = MakeFromString<ClientInfo>(str);
    TEST_CHECK_STAGE(ClientInfo2 == ClientInfo());

    return TEST_SUCCESS;
}


template<typename CharT>
size_t basic_serializers_and_parsers_test()
{
    std::basic_string<CharT> text = Tools::basic_string_cast<CharT>("Hello there, I am a test text!");
    std::string str = Serializer<std::basic_string<CharT>>(&text).ToString();
    std::basic_string<CharT> text2 = MakeFromString<std::basic_string<CharT>>(str);
    TEST_CHECK_STAGE(text == text2);

    str.resize(str.size()-1);
    text2 = MakeFromString<std::basic_string<CharT>>(str);
    TEST_CHECK_STAGE(text2 == std::basic_string<CharT>());

    text = Tools::basic_string_cast<CharT>("Hello there, I am a test message!");
    const BasicMessage<CharT> BasicMessage1(text);
    BasicMessage<CharT> BasicMessage2;
    str = BasicMessage1.GetSerializer()->ToString();
    BasicMessage2 = MakeFromString<BasicMessage<CharT>>(str);
    TEST_CHECK_STAGE(BasicMessage2 == BasicMessage1);

    str = str.substr(0, str.size()-1);
    BasicMessage2 = MakeFromString<BasicMessage<CharT>>(str);
    TEST_CHECK_STAGE(BasicMessage2 == BasicMessage<CharT>());

    return TEST_SUCCESS;
}

size_t requests_serializers_and_parsers_test()
{
    UserID_t id = 123123414;
    const ClientInfo ClientInfo1(id);
          ClientInfo ClientInfo2;
    const ClientInfoRequest clientInfoRequest;
    *clientInfoRequest.GetClientInfo() = ClientInfo1;
    std::string str = clientInfoRequest.GetSerializer()->ToString();
    auto parser = Parser<Request>();
    auto iter = parser.FromString(str);
    TEST_CHECK_STAGE(iter != str.begin());
    
	#undef GetObject
    TEST_CHECK_STAGE(parser.GetObject()->GetType() == clientInfoRequest.GetType());

    Request* pRequest = parser.GetObject().get();
    ClientInfoRequest* pClientInfoRequest = static_cast<ClientInfoRequest*>(pRequest);
    TEST_CHECK_STAGE(*pClientInfoRequest->GetClientInfo() == ClientInfo1);

    str.resize(str.size()-1);
    iter = Parser<Request>().FromString(str);
    TEST_CHECK_STAGE(iter == str.begin());

    auto text = Tools::basic_string_cast<CHAT_CHAR>("Hello there, I am a test nickname!");
    const Message Message1(text);
          Message Message2;
    const MessageRequest messageRequest;
    
	#undef GetMessage
    *messageRequest.GetMessage() = Message1;
    str = messageRequest.GetSerializer()->ToString();
    iter = parser.FromString(str);
    TEST_CHECK_STAGE(iter != str.begin());

    TEST_CHECK_STAGE(parser.GetObject()->GetType() == ERequestType::Message);

    pRequest = parser.GetObject().get();
    MessageRequest* pMessageRequest = static_cast<MessageRequest*>(pRequest);
    TEST_CHECK_STAGE(*pMessageRequest->GetMessage() == Message1);

    str.resize(str.size()-1);
    iter = Parser<Request>().FromString(str);
    TEST_CHECK_STAGE(iter == str.begin());

    return TEST_SUCCESS;
}

size_t version_conversation_test()
{
    
    Version_t ver = ConvertVersions(NICKSVCHAT_VERSION_MAJOR, NICKSVCHAT_VERSION_MINOR, NICKSVCHAT_VERSION_PATCH, 0);
    TEST_CHECK_STAGE(ConvertVersionToMajor(ver) == NICKSVCHAT_VERSION_MAJOR);
    TEST_CHECK_STAGE(ConvertVersionToMinor(ver) == NICKSVCHAT_VERSION_MINOR);
    TEST_CHECK_STAGE(ConvertVersionToPatch(ver) == NICKSVCHAT_VERSION_PATCH);
    TEST_CHECK_STAGE(ConvertVersionToTweak(ver) == 0);

    return TEST_SUCCESS;
}


size_t requests_test()
{
    
    Request Req;
    MessageRequest mReq;
    ClientInfoRequest ciReq;
    TEST_CHECK_STAGE(Req.GetType() == ERequestType::Unknown);
    TEST_CHECK_STAGE(mReq.GetType() == ERequestType::Message);
    TEST_CHECK_STAGE(ciReq.GetType() == ERequestType::ClientInfo);

    


    
    return TEST_SUCCESS;
}


Message g_Message;

class TestChatServerException : public std::exception
{
    const char * what() const noexcept override 
    {
        return "TestChatServerException got bad request";
    }
};

class TestChatServer : public ChatServer
{
    void OnHandleMessageRequest(const MessageRequest* pcRer, RequestInfo reqInfo,  NickSV::Chat::EResult outsideResult) override
    {
        if(outsideResult != NickSV::Chat::EResult::Success)
            return;

        g_Message = *pcRer->GetMessage();
    }

    // If we do not require the client to send their information,
    // set client's state as Active to allow message exchange
	void OnAcceptClient(ConnectionInfo* pInfo, ClientInfo* pClientInfo, NickSV::Chat::EResult res) override
    {
        pClientInfo->GetState() = EState::Active;
    }

	void OnBadIncomingRequest(std::string strReq, NotNull<ClientInfo*> pClientInfo, NickSV::Chat::EResult res) override
    {
        throw TestChatServerException();
    }
};

size_t client_server_data_exchange_test()
{
    
    TestChatServer server;
    ChatClient client;
    MessageRequest req;
    *req.GetMessage() = Message(_T("Hello guys, ima test message"));
    TEST_CHECK_STAGE(server.Run() != NickSV::Chat::EResult::Error);

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    bool result = true;
    if(client.Run() == NickSV::Chat::EResult::Error)
    {
        server.CloseSocket();
        server.Wait();
        result = false;
    }
    TEST_CHECK_STAGE(result);

    if( client.QueueRequest(&req, {0, 0}) != NickSV::Chat::EResult::Success)
    {
        server.CloseSocket();
        client.CloseSocket();
        server.Wait();
        client.Wait();
        result = false;
    }
    TEST_CHECK_STAGE(result);

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    server.CloseSocket();
    client.CloseSocket();
    server.Wait();
    client.Wait();
    TEST_CHECK_STAGE(g_Message == Message(_T("Hello guys, ima test message")));

    
    return TEST_SUCCESS;
}


//#include <winsock2.h>
//#include <iphlpapi.h>

int main(int arc, const char ** argv)
{
    static_assert(Tools::is_char<char>::value);
    static_assert(Tools::is_char<wchar_t>::value);
    static_assert(Tools::is_char<char16_t>::value);
    static_assert(Tools::is_char<char32_t>::value);

    static_assert(Tools::is_char<const char>::value);
    static_assert(Tools::is_char<volatile wchar_t>::value);
    static_assert(Tools::is_char<const volatile char16_t>::value);

    static_assert(!Tools::is_char<int>::value);

    TEST_VERIFY(client_info_test());
    TEST_VERIFY(client_info_serializers_and_parsers_test());

    TEST_VERIFY(basic_serializers_and_parsers_test<char>());
    TEST_VERIFY(basic_serializers_and_parsers_test<wchar_t>());
    TEST_VERIFY(basic_serializers_and_parsers_test<char16_t>());
    TEST_VERIFY(basic_serializers_and_parsers_test<char32_t>());

    TEST_VERIFY(requests_serializers_and_parsers_test());

    TEST_VERIFY(version_conversation_test());

    TEST_VERIFY(requests_test());

    TEST_VERIFY(client_server_data_exchange_test());

    std::cout << '\n' << Tools::Testing::TestsFailed << " subtests failed" << std::endl;
    
    return Tools::Testing::TestsFailed;
}