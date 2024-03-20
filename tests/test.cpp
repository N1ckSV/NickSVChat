
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

#include <iostream>
#include <iomanip>
#include <exception>
#include <type_traits>
#include <thread>

#define SETW_VAL 50

#define IF_RETURN(exp, ret) if(exp) { return (ret); } else { (void)(ret); }
#define IFN_RETURN(exp, ret) if(!(exp)) { return (ret); }

#define RESULT(stage, failed)                                              \
    try {                                                                  \
        auto stageOut = stage;                                             \
        std::cout << std::setw(SETW_VAL) << std::string(#stage) << ": ";   \
        if(!stageOut) {                                                    \
            std::cout << std::string("PASSED") << std::endl;               \
        }                                                                  \
        else {                                                             \
            failed++;                                                      \
            std::cout  << std::string("FAILED on stage ")                  \
            << stageOut << std::endl;                                      \
        }                                                                  \
    } catch (const std::exception& ex) {                                   \
        std::cout << "Exception thrown - " << ex.what() << std::endl;      \
    }                                                                      \



using namespace NickSV::Chat;

int client_info_test()
{
    int stage = 0;
    ClientInfo info1, info2;
    info1.GetUserID() = 12345;
    info2.GetUserID() = 12345;
    IF_RETURN(info1 != info2, ++stage);

    info2.GetUserID() = 10101;
    IF_RETURN(info1 == info2, ++stage);

    info1.GetUserID() = 12345;
    IF_RETURN(info1 == info2, ++stage);

    return 0;
}


int client_info_serializers_and_parsers_test()
{
    int stage = 0;
    UserID_t id = 2;
    const auto ClientInfo1 = ClientInfo(id);
    ClientInfo ClientInfo2;
    auto str = ClientInfo1.GetSerializer()->ToString();
    ClientInfo2 = MakeFromString<ClientInfo>(str);
    IF_RETURN(ClientInfo2 != ClientInfo1, ++stage);

    str.resize(str.size()-1);
    ClientInfo2 = MakeFromString<ClientInfo>(str);
    IF_RETURN(ClientInfo2 != ClientInfo(), ++stage);

    return 0;
}


template<typename CharT>
int basic_serializers_and_parsers_test()
{
    int stage = 0;
    std::basic_string<CharT> text = basic_string_cast<CharT>("Hello there, I am a test text!");
    std::string str = Serializer<std::basic_string<CharT>>(&text).ToString();
    std::basic_string<CharT> text2 = MakeFromString<std::basic_string<CharT>>(str);
    IF_RETURN(text != text2, ++stage);

    str.resize(str.size()-1);
    text2 = MakeFromString<std::basic_string<CharT>>(str);
    IF_RETURN(text2 != std::basic_string<CharT>(), ++stage);

    text = basic_string_cast<CharT>("Hello there, I am a test message!");
    const BasicMessage<CharT> BasicMessage1(text);
    BasicMessage<CharT> BasicMessage2;
    str = BasicMessage1.GetSerializer()->ToString();
    BasicMessage2 = MakeFromString<BasicMessage<CharT>>(str);
    IF_RETURN(BasicMessage2 != BasicMessage1, ++stage);

    str = str.substr(0, str.size()-1);
    BasicMessage2 = MakeFromString<BasicMessage<CharT>>(str);
    IF_RETURN(BasicMessage2 != BasicMessage<CharT>(), ++stage);

    return 0;
}

int requests_serializers_and_parsers_test()
{
    int stage = 0;
    UserID_t id = 123123414;
    const ClientInfo ClientInfo1(id);
          ClientInfo ClientInfo2;
    const ClientInfoRequest clientInfoRequest;
    *clientInfoRequest.GetClientInfo() = ClientInfo1;
    std::string str = clientInfoRequest.GetSerializer()->ToString();
    auto parser = Parser<Request>();
    auto iter = parser.FromString(str);
    IF_RETURN(iter == str.begin(), ++stage);

    IF_RETURN(parser.GetObject()->GetType() != clientInfoRequest.GetType(), ++stage);

    Request* pRequest = parser.GetObject().get();
    ClientInfoRequest* pClientInfoRequest = static_cast<ClientInfoRequest*>(pRequest);
    IF_RETURN(*pClientInfoRequest->GetClientInfo() != ClientInfo1, ++stage);

    str.resize(str.size()-1);
    iter = Parser<Request>().FromString(str);
    IF_RETURN(iter != str.begin(), ++stage);

    auto text = basic_string_cast<CHAT_CHAR>("Hello there, I am a test nickname!");
    const Message Message1(text);
          Message Message2;
    const MessageRequest messageRequest;
    *messageRequest.GetMessage() = Message1;
    str = messageRequest.GetSerializer()->ToString();
    iter = parser.FromString(str);
    IF_RETURN(iter == str.begin(), ++stage);

    IF_RETURN(parser.GetObject()->GetType() != ERequestType::Message, ++stage);

    pRequest = parser.GetObject().get();
    MessageRequest* pMessageRequest = static_cast<MessageRequest*>(pRequest);
    IF_RETURN(*pMessageRequest->GetMessage() != Message1, ++stage);

    str.resize(str.size()-1);
    iter = Parser<Request>().FromString(str);
    IF_RETURN(iter != str.begin(), ++stage);

    return 0;
}

int version_conversation_test()
{
    int stage = 0;
    Version_t ver = ConvertVersions(NICKSVCHAT_VERSION_MAJOR, NICKSVCHAT_VERSION_MINOR, NICKSVCHAT_VERSION_PATCH, 0);
    IF_RETURN(ConvertVersionToMajor(ver) != NICKSVCHAT_VERSION_MAJOR, ++stage);
    IF_RETURN(ConvertVersionToMinor(ver) != NICKSVCHAT_VERSION_MINOR, ++stage);
    IF_RETURN(ConvertVersionToPatch(ver) != NICKSVCHAT_VERSION_PATCH, ++stage);
    IF_RETURN(ConvertVersionToTweak(ver) != 0, ++stage);

    return 0;
}


int requests_test()
{
    int stage = 0;
    Request Req;
    MessageRequest mReq;
    ClientInfoRequest ciReq;
    IF_RETURN(Req.GetType() != ERequestType::Unknown, ++stage);
    IF_RETURN(mReq.GetType() != ERequestType::Message, ++stage);
    IF_RETURN(ciReq.GetType() != ERequestType::ClientInfo, ++stage);

    


    
    return 0;
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
    void OnHandleRequest(const Request* pcR, RequestInfo rInfo, NickSV::Chat::EResult outsideResult) override
    {
        if(pcR->GetType() == ERequestType::Message)
            g_Message = *static_cast<const MessageRequest*>(pcR)->GetMessage();
    }
    void OnBadIncomingRequest(std::string str, UserID_t, NickSV::Chat::EResult outsideResult) override
    {
        throw TestChatServerException();
    }
    // If we do not require the client to send their information,
    // set client's state as Active to allow message exchange
    void OnAcceptClient(ConnectionInfo* pInfo, UserID_t id, NickSV::Chat::EResult res)
    {
        GetClientInfo(id).GetState() = EState::Active;
    }
};

int client_server_data_exchange_test()
{
    int stage = 0;
    TestChatServer server;
    ChatClient client;
    MessageRequest req;
    *req.GetMessage() = Message(TEXT("Hello guys, ima test message"));
    IF_RETURN(server.Run() == NickSV::Chat::EResult::Error, ++stage);

    ++stage;
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    if(client.Run() == NickSV::Chat::EResult::Error)
    {
        server.CloseSocket();
        server.Wait();
        return stage;
    }

    ++stage;
    if( client.QueueRequest(&req, {0, 0}) != NickSV::Chat::EResult::Success)
    {
        server.CloseSocket();
        client.CloseSocket();
        server.Wait();
        client.Wait();
        return stage;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    server.CloseSocket();
    client.CloseSocket();
    server.Wait();
    client.Wait();
    IF_RETURN(g_Message != Message(TEXT("Hello guys, ima test message")), ++stage);

    
    return 0;
}


//#include <winsock2.h>
//#include <iphlpapi.h>

int main(int arc, const char ** argv)
{
    int ntest_failed = 0;

    static_assert(is_char<char>::value);
    static_assert(is_char<wchar_t>::value);
    static_assert(is_char<char16_t>::value);
    static_assert(is_char<char32_t>::value);

    static_assert(is_char<const char>::value);
    static_assert(is_char<volatile wchar_t>::value);
    static_assert(is_char<const volatile char16_t>::value);

    static_assert(!is_char<int>::value);

    RESULT(client_info_test(), ntest_failed);
    RESULT(client_info_serializers_and_parsers_test(), ntest_failed);

    RESULT(basic_serializers_and_parsers_test<char>(), ntest_failed);
    RESULT(basic_serializers_and_parsers_test<wchar_t>(), ntest_failed);
    RESULT(basic_serializers_and_parsers_test<char16_t>(), ntest_failed);
    RESULT(basic_serializers_and_parsers_test<char32_t>(), ntest_failed);

    RESULT(requests_serializers_and_parsers_test(), ntest_failed);

    RESULT(version_conversation_test(), ntest_failed);

    RESULT(requests_test(), ntest_failed);

    RESULT(client_server_data_exchange_test(), ntest_failed);

/////////////////////////////////////////////////////////////////////////
    //ULONG bufferSize = 1500;
    //PMIB_IPADDRTABLE  pIPAddrTable;
    //DWORD dwSize = 0;
    //DWORD dwRetVal = 0;
    //IN_ADDR IPAddr;
//
//
    //
    //PMIB_IPADDRTABLE pCurrAddresses = NULL;
//
    //size_t Iterations = 0;
    //do {
    //    pIPAddrTable = (MIB_IPADDRTABLE *) malloc(bufferSize);
    //    if (pIPAddrTable == NULL) {
    //        dwRetVal = 1231;
    //        printf
    //            ("Memory allocation failed for IP_ADAPTER_ADDRESSES struct\n");
    //        break;
    //    }
//
    //    dwRetVal =
    //        GetIpAddrTable(pIPAddrTable, &bufferSize, FALSE);
//
    //    if (dwRetVal == ERROR_INSUFFICIENT_BUFFER) {
    //        free(pIPAddrTable);
    //        pIPAddrTable = NULL;
    //    } else {
    //        break;
    //    }
//
    //    Iterations++;
//
    //} while ((dwRetVal == ERROR_INSUFFICIENT_BUFFER) && (Iterations < 10));
    //setlocale(LC_ALL, "");
    //unsigned int i = 0;
    //if (dwRetVal == NO_ERROR) {
    //    // If successful, output some information from the data we received
    //    printf("\tNum Entries: %ld\n", pIPAddrTable->dwNumEntries);
    //    for (i=0; i < (int) pIPAddrTable->dwNumEntries; i++) {
    //        printf("\n\tInterface Index[%d]:\t%ld\n", i, pIPAddrTable->table[i].dwIndex);
    //        IPAddr.S_un.S_addr = (u_long) pIPAddrTable->table[i].dwAddr;
    //        printf("\tIP Address[%d]:     \t%s\n", i, inet_ntoa(IPAddr) );
    //        IPAddr.S_un.S_addr = (u_long) pIPAddrTable->table[i].dwMask;
    //        printf("\tSubnet Mask[%d]:    \t%s\n", i, inet_ntoa(IPAddr) );
    //        IPAddr.S_un.S_addr = (u_long) pIPAddrTable->table[i].dwBCastAddr;
    //        printf("\tBroadCast[%d]:      \t%s (%ld%)\n", i, inet_ntoa(IPAddr), pIPAddrTable->table[i].dwBCastAddr);
    //        printf("\tReassembly size[%d]:\t%ld\n", i, pIPAddrTable->table[i].dwReasmSize);
    //        printf("\tType and State[%d]:", i);
    //        if (pIPAddrTable->table[i].wType & MIB_IPADDR_PRIMARY)
    //            printf("\tPrimary IP Address");
    //        if (pIPAddrTable->table[i].wType & MIB_IPADDR_DYNAMIC)
    //            printf("\tDynamic IP Address");
    //        if (pIPAddrTable->table[i].wType & MIB_IPADDR_DISCONNECTED)
    //            printf("\tAddress is on disconnected interface");
    //        if (pIPAddrTable->table[i].wType & MIB_IPADDR_DELETED)
    //            printf("\tAddress is being deleted");
    //        if (pIPAddrTable->table[i].wType & MIB_IPADDR_TRANSIENT)
    //            printf("\tTransient address");
    //        printf("\n");
    //        }
    //} else {
    //    std::cout << "Something went wrong, error code: " << dwRetVal << std::endl;
    //}
//
//
//
    //free(pIPAddrTable);
/////////////////////////////////////////////////////////////////////////

    //Subtest, because whole exe is CTest test
    std::cout << '\n' << ntest_failed << " subtests failed" << std::endl;
    
    return ntest_failed;
}