
#include "NickSV/Chat/Utils.h"
#include "NickSV/Chat/BasicClientInfo.h"
#include "NickSV/Chat/BasicMessage.h"
#include "NickSV/Chat/Serializers/BClientInfoSerializer.h"
#include "NickSV/Chat/Serializers/BMessageSerializer.h"
#include "NickSV/Chat/Serializers/BStringSerializer.h"
#include "NickSV/Chat/ChatClient.h"
#include "NickSV/Chat/ChatServer.h"
#include "NickSV/Chat/Requests/Request.h"
#include "NickSV/Chat/Requests/MessageRequest.h"
#include "NickSV/Chat/Requests/ClientInfoRequest.h"
#include <iostream>
#include <iomanip>
#include <exception>
#include <type_traits>

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

template<typename CharT>
int basic_client_info_test()
{
    int stage = 0;
    BasicClientInfo<CharT> invalid;
    invalid.SetInvalid();
    IF_RETURN(invalid.IsValid(), ++stage);

    BasicClientInfo<CharT> info1, info2;
    info1.GetNickname() = basic_string_cast<CharT>("nick1");
    info2.GetNickname() = basic_string_cast<CharT>("nick1");
    IF_RETURN(info1 != info2, ++stage);

    info2.GetNickname() = basic_string_cast<CharT>("nick2");
    IF_RETURN(info1 == info2, ++stage);

    info1.GetNickname() = basic_string_cast<CharT>("nick1");
    info2.SetInvalid();
    IF_RETURN(info1 == info2, ++stage);

    return 0;
}



template<typename CharT>
int basic_client_info_serializer_test()
{
    int stage = 0;
    std::basic_string<CharT> nick = basic_string_cast<CharT>("Hello there, I am a test nickname!");
    const BasicClientInfo<CharT> BasicClientInfo1(nick);
    BasicClientInfo<CharT> BasicClientInfo2;
    std::string str = BasicClientInfo1.GetSerializer()->ToString();
    BasicClientInfo2 = MakeFromString<BasicClientInfo<CharT>>(str);
    IF_RETURN(BasicClientInfo2 != BasicClientInfo1, ++stage);

    str = str.substr(0, str.size()-1);
    BasicClientInfo2 = MakeFromString<BasicClientInfo<CharT>>(str);
    IF_RETURN(BasicClientInfo2 != BasicClientInfo<CharT>(), ++stage);

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


template<typename CharT>
int basic_message_serializer_test()
{
    int stage = 0;
    std::basic_string<CharT> text = basic_string_cast<CharT>("Yo yo yo text here!");
    const BasicMessage<CharT> BasicMessage1(text);
    BasicMessage<CharT> BasicMessage2;
    std::string str = BasicMessage1.GetSerializer()->ToString();
    BasicMessage2 = MakeFromString<BasicMessage<CharT>>(str);
    IF_RETURN(BasicMessage2 != BasicMessage1, ++stage);

    str = str.substr(0, str.size()-1);
    BasicMessage2 = MakeFromString<BasicMessage<CharT>>(str);
    IF_RETURN(BasicMessage2 != BasicMessage<CharT>(), ++stage);

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


#define DEFAULT_SERVER_PORT 27020

int client_server_data_exchange_test()
{
    //int stage = 0;
    //ChatServer server;
    //ChatClient client;
    //SteamNetworkingIPAddr localhost;
    //localhost.Clear();
    //IF_RETURN(!localhost.ParseString("127.0.0.1"), ++stage);
//
    //IF_RETURN(!localhost.IsIPv4(), ++stage);
//
    //IF_RETURN(!localhost.IsLocalHost(), ++stage);
//
    //localhost.m_port = DEFAULT_SERVER_PORT;

    
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

    RESULT(basic_client_info_test<char>(), ntest_failed);
    RESULT(basic_client_info_test<wchar_t>(), ntest_failed);
    RESULT(basic_client_info_test<char16_t>(), ntest_failed);
    RESULT(basic_client_info_test<char32_t>(), ntest_failed);

    RESULT(basic_client_info_serializer_test<char>(), ntest_failed);
    RESULT(basic_client_info_serializer_test<wchar_t>(), ntest_failed);
    RESULT(basic_client_info_serializer_test<char16_t>(), ntest_failed);
    RESULT(basic_client_info_serializer_test<char32_t>(), ntest_failed);

    RESULT(basic_message_serializer_test<char>(), ntest_failed);
    RESULT(basic_message_serializer_test<wchar_t>(), ntest_failed);
    RESULT(basic_message_serializer_test<char16_t>(), ntest_failed);
    RESULT(basic_message_serializer_test<char32_t>(), ntest_failed);

    RESULT(version_conversation_test(), ntest_failed);

    RESULT(requests_test(), ntest_failed);

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