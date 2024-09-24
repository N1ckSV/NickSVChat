

#include <type_traits>
#include <thread>




#define TEST_IGNORE_PRINT_ON_SUCCESS

#include "NickSV/Tools/Testing.h"
#include "NickSV/Tools/ValueLock.h"


#include "NickSV/Chat/Utils.h"
#include "NickSV/Chat/ChatClient.h"
#include "NickSV/Chat/ChatServer.h"
#include "NickSV/Chat/Message.h"

using namespace NickSV;
using namespace NickSV::Chat;

static size_t client_info_test()
{
    ClientInfo info1, info2;
    info1.SetUserID(12345);
    info2.SetUserID(12345);
    TEST_CHECK_STAGE(info1.UserID() == info2.UserID());

    info2.SetUserID(10101);
    TEST_CHECK_STAGE(info1.UserID() != info2.UserID());

    return TEST_SUCCESS;
}

//COMMENTED bc ClientInfo have no access to serialization/parsing it is to ClientInfoRequest
//static size_t client_info_serializers_and_parsers_test()
//{
//    UserID_t id = 2;
//    ClientInfo ClientInfo1;
//    ClientInfo1.SetUserID(id);
//    std::string str;
//    TEST_CHECK_STAGE(ClientInfo1.SerializeTo(&str));
//
//    ClientInfo ClientInfo2;
//    TEST_CHECK_STAGE(ClientInfo2.ParseFrom(str));
//
//    TEST_CHECK_STAGE(ClientInfo2.UserID() == ClientInfo1.UserID());
//
//    TEST_CHECK_STAGE(ClientInfo2.State() == ClientInfo1.State());
//
//    TEST_CHECK_STAGE(ClientInfo2.LibVersion() == ClientInfo1.LibVersion());
//
//    str.resize(str.size() >> 1);
//    TEST_CHECK_STAGE(!ClientInfo2.ParseFrom(str));
//
//    return TEST_SUCCESS;
//}



static size_t requests_serializers_and_parsers_test()
{
    UserID_t id = 123123414;
    Version_t ver = 536432;
    EState state = EState::Active;
    ClientInfo ClientInfo1;
    ClientInfo ClientInfo2;
    ClientInfo1.SetUserID(id);
    ClientInfo1.SetState(state);
    ClientInfoRequest clientInfoRequest;
    ClientInfoRequest clientInfoRequest2;
    clientInfoRequest.PackClientInfoFrom(ClientInfo1);
    std::string str;
    TEST_CHECK_STAGE(clientInfoRequest.SerializeToString(&str));
    
    TEST_CHECK_STAGE(clientInfoRequest2.ParseFromString(str));
    
    TEST_CHECK_STAGE(clientInfoRequest.Type() == clientInfoRequest2.Type());
    
    TEST_CHECK_STAGE(clientInfoRequest2.HasClientInfo());

    clientInfoRequest2.UnpackClientInfoTo(&ClientInfo2);
    TEST_CHECK_STAGE(ClientInfo2.UserID() == ClientInfo1.UserID());

    TEST_CHECK_STAGE(ClientInfo2.State() == ClientInfo1.State());

    TEST_CHECK_STAGE(ClientInfo2.LibVersion() == ClientInfo1.LibVersion());









    UserID_t sender_id = 4235324;
    Message Message1;
    Message Message2;
    Message1.SetSenderID(sender_id);
    Message1.SetText(u8"Test message");
    MessageRequest MessageRequest1;
    MessageRequest MessageRequest2;
    MessageRequest1.PackMessageFrom(Message1);
    TEST_CHECK_STAGE(MessageRequest1.SerializeToString(&str));
    
    TEST_CHECK_STAGE(MessageRequest2.ParseFromString(str));
    
    TEST_CHECK_STAGE((MessageRequest1.Type() == MessageRequest2.Type()) && MessageRequest1.Type() == ERequestType::Message);
    
    TEST_CHECK_STAGE(MessageRequest2.HasMessage());

    MessageRequest2.UnpackMessageTo(&Message2);
    TEST_CHECK_STAGE(Message2.SenderID() == Message1.SenderID());

    TEST_CHECK_STAGE(Message2.Text() == Message1.Text());

    //TODO FIXME ADD TEXT HERE

    

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

class TestChatServerException : public std::exception
{
    const char * what() const noexcept override 
    {
        return "TestChatServerException got bad request";
    }
};

class TestChatServer : public ChatServer
{
public:
    std::string MessageText;
    bool GotMessage = false;
	using ChatServer::OnHandleRequest;
	
    void OnHandleRequest(const MessageRequest& rcRer, RequestInfo,
                                 NickSV::Chat::EResult outsideResult,
                                 TaskInfo) override
    {
        if(outsideResult != NickSV::Chat::EResult::Success)
            return;

        Message message;
        bool unpackResult = rcRer.UnpackMessageTo(&message);
        CHAT_ASSERT(unpackResult, something_went_wrong_ERROR_MESSAGE);
        MessageText = std::move(*message.MutableText());
        GotMessage = true;
    }

	void OnBadIncomingRequest(std::string, ClientInfo&, NickSV::Chat::EResult) override
    {
        throw TestChatServerException();
    }
};

#include <chrono>

static size_t client_server_data_exchange_test()
{
    using namespace std::chrono;
    TestChatServer server;
    ChatClient client;
    MessageRequest req;
    Message message;
    message.SetSenderID(1231);
    message.SetText(u8"Hello guys, ima test message");
    req.PackMessageFrom(message);
    TEST_CHECK_STAGE(server.Run() != NickSV::Chat::EResult::Error);

    bool result = true;
    if(client.Run() == NickSV::Chat::EResult::Error)
    {
        server.CloseSocket();
        server.Wait();
        result = false;
    }
    TEST_CHECK_STAGE(result);

    auto t1 = high_resolution_clock::now();
    while(client.GetClientInfo().State() != EState::Active &&
          duration_cast<milliseconds>(high_resolution_clock::now()-t1) <
          milliseconds(1000))
              std::this_thread::sleep_for(milliseconds(10));
    if(client.GetClientInfo().State() != EState::Active)
    {
        server.CloseSocket();
        client.CloseSocket();
        server.Wait();
        client.Wait();
        result = false;
    }
    TEST_CHECK_STAGE(result);

    if(client.QueueRequest(req).GetFutureResult() != NickSV::Chat::EResult::Success)
    {
        server.CloseSocket();
        client.CloseSocket();
        server.Wait();
        client.Wait();
        result = false;
    }
    TEST_CHECK_STAGE(result);

    t1 = high_resolution_clock::now();
    while(!server.GotMessage &&
          duration_cast<milliseconds>(high_resolution_clock::now()-t1) <
          milliseconds(1000))
              std::this_thread::sleep_for(milliseconds(10));
    server.CloseSocket();
    client.CloseSocket();
    server.Wait();
    client.Wait();
    TEST_CHECK_STAGE(server.MessageText == u8"Hello guys, ima test message");

    
    return TEST_SUCCESS;
}


//#include <winsock2.h>
//#include <iphlpapi.h>

int main(int, const char **)
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    TEST_VERIFY(client_info_test());
    //TEST_VERIFY(client_info_serializers_and_parsers_test()); //read dscrp

    TEST_VERIFY(requests_serializers_and_parsers_test());

    TEST_VERIFY(version_conversation_test());

    TEST_VERIFY(client_server_data_exchange_test());

    std::cout << '\n' << NickSV::Tools::Testing::TestsFailed << " subtests failed\n";
    
    return NickSV::Tools::Testing::TestsFailed;
}