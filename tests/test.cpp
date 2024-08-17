

#include <type_traits>
#include <thread>


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

#define TEST_IGNORE_PRINT_ON_SUCCESS

#include "NickSV/Tools/Testing.h"


using namespace NickSV;
using namespace NickSV::Chat;

static size_t client_info_test()
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


static size_t client_info_serializers_and_parsers_test()
{
    UserID_t id = 2;
    const auto ClientInfo1 = ClientInfo(id);
    auto str = ClientInfo1.GetSerializer()->ToString();
    auto parser = Parser<ClientInfo>();
    TEST_CHECK_STAGE(std::distance(str.cbegin(), parser.FromString(str)) > 0);

    auto ClientInfo2 = parser.GetObject();
    TEST_CHECK_STAGE(ClientInfo2 == ClientInfo1);

    str.resize(str.size()-1);
    ClientInfo2 = MakeFromString<ClientInfo>(str);
    TEST_CHECK_STAGE(ClientInfo2 == ClientInfo());

    return TEST_SUCCESS;
}


template<typename CharT>
size_t basic_serializers_and_parsers_test()
{
    std::basic_string<CharT> text = TEXT("Hello there, I am a test text!", CharT);
    std::string str = Serializer<std::basic_string<CharT>>(&text).ToString();
    std::basic_string<CharT> text2 = MakeFromString<std::basic_string<CharT>>(str);
    TEST_CHECK_STAGE(text == text2);

    str.resize(str.size()-1);
    text2 = MakeFromString<std::basic_string<CharT>>(str);
    TEST_CHECK_STAGE(text2 == std::basic_string<CharT>());

    text = TEXT("Hello there, I am a test message!", CharT);
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

static size_t requests_serializers_and_parsers_test()
{
    UserID_t id = 123123414;
    const ClientInfo ClientInfo1(id);
          ClientInfo ClientInfo2;
    const ClientInfoRequest clientInfoRequest(ClientInfo1);
    std::string str = clientInfoRequest.GetSerializer()->ToString();
    auto parser = Parser<Request>();
    auto iter = parser.FromString(str);
    TEST_CHECK_STAGE(iter != str.begin());
    
	#undef GetObject
    TEST_CHECK_STAGE(parser.GetObject().GetType() == clientInfoRequest.GetType());

    Request* pRequest = &(parser.GetObject());
    ClientInfoRequest* pClientInfoRequest = static_cast<ClientInfoRequest*>(pRequest);
    TEST_CHECK_STAGE(pClientInfoRequest->GetClientInfo() == ClientInfo1);

    str.resize(str.size()-1);
    iter = Parser<Request>().FromString(str);
    TEST_CHECK_STAGE(iter == str.begin());

    auto text = TEXT("Hello there, I am a test nickname!", CHAT_CHAR);
    const Message Message1(text);
          Message Message2;
    const MessageRequest messageRequest(Message1);
	#undef GetMessage
    str = messageRequest.GetSerializer()->ToString();
    iter = parser.FromString(str);
    TEST_CHECK_STAGE(iter != str.begin());

    TEST_CHECK_STAGE(parser.GetObject().GetType() == ERequestType::Message);

    pRequest = &(parser.GetObject());
    MessageRequest* pMessageRequest = static_cast<MessageRequest*>(pRequest);
    TEST_CHECK_STAGE(pMessageRequest->GetMessage() == Message1);

    str.resize(str.size()-1);
    iter = parser.FromString(str);
    MessageRequest* pReq = static_cast<MessageRequest*>(&(parser.GetObject()));
    (void)pReq;
    TEST_CHECK_STAGE(iter == str.begin());

    return TEST_SUCCESS;
}

static size_t version_conversation_test()
{
    
    Version_t ver = ConvertVersions(NICKSVCHAT_VERSION_MAJOR, NICKSVCHAT_VERSION_MINOR, NICKSVCHAT_VERSION_PATCH, 0);
    TEST_CHECK_STAGE(ConvertVersionToMajor(ver) == NICKSVCHAT_VERSION_MAJOR);
    TEST_CHECK_STAGE(ConvertVersionToMinor(ver) == NICKSVCHAT_VERSION_MINOR);
    TEST_CHECK_STAGE(ConvertVersionToPatch(ver) == NICKSVCHAT_VERSION_PATCH);
    TEST_CHECK_STAGE(ConvertVersionToTweak(ver) == 0);

    return TEST_SUCCESS;
}


static size_t requests_test()
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
	using ChatServer::OnHandleRequest;
	
    void OnHandleRequest(const MessageRequest& rcRer, RequestInfo,
                                 NickSV::Chat::EResult outsideResult,
                                 TaskInfo) override
    {
        if(outsideResult != NickSV::Chat::EResult::Success)
            return;

        g_Message = rcRer.GetMessage();
    }

	void OnBadIncomingRequest(std::string, ClientInfo&, NickSV::Chat::EResult) override
    {
        throw TestChatServerException();
    }
};

static size_t client_server_data_exchange_test()
{
    
    TestChatServer server;
    ChatClient client;
    MessageRequest req;
    req.GetMessage() = Message(_T("Hello guys, ima test message"));
    TEST_CHECK_STAGE(server.Run() != NickSV::Chat::EResult::Error);

    bool result = true;
    if(client.Run() == NickSV::Chat::EResult::Error)
    {
        server.CloseSocket();
        server.Wait();
        result = false;
    }
    TEST_CHECK_STAGE(result);

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    if( client.QueueRequest(req).GetFutureResult() != NickSV::Chat::EResult::Success)
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
    TEST_CHECK_STAGE(g_Message.GetText() == _T("Hello guys, ima test message"));

    
    return TEST_SUCCESS;
}


//#include <winsock2.h>
//#include <iphlpapi.h>

int main(int, const char **)
{
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

    std::cout << '\n' << NickSV::Tools::Testing::TestsFailed << " subtests failed\n";
    
    return NickSV::Tools::Testing::TestsFailed;
}