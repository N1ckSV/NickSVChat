
#include "Defines.h"
#include "ClientInfo.h"
#include "Serializers.h"
#include <iostream>
#include <iomanip>

#define SETW_VAL 40

#define IF_RETURN(exp, ret) if(exp) { return (ret); }

#define RESULT(stage, failed) std::cout << std::setw(SETW_VAL) << std::string(#stage);         \
                            if(!stage) {                                                       \
                                std::cout << std::string(": PASSED") << std::endl;             \
                            }                                                                  \
                            else {                                                             \
                                failed++;                                                      \
                                std::cout  << std::string(": FAILED on stage ")                \
                                << stage << std::endl;                                         \
                            }                                                                  \



template<typename CharT>
int client_info_test()
{
    int stage = 1;
    NickSV::Chat::ClientInfo<CharT> invalid;
    invalid.SetInvalid();
    IF_RETURN(invalid.IsValid(), stage);
    stage++;

    NickSV::Chat::ClientInfo<CharT> info1, info2;
    info1.m_sNick = NickSV::Chat::basic_string_cast<CharT>("nick1");
    info2.m_sNick = NickSV::Chat::basic_string_cast<CharT>("nick1");
    IF_RETURN(info1 != info2, stage);
    stage++;

    info2.m_sNick = NickSV::Chat::basic_string_cast<CharT>("nick2");
    IF_RETURN(info1 == info2, stage);
    stage++;

    info1.m_sNick = NickSV::Chat::basic_string_cast<CharT>("nick1");
    info2.SetInvalid();
    IF_RETURN(info1 == info2, stage);
    stage++;

    return 0;
}



template<typename CharT>
int client_info_serializer_test()
{
    int stage = 1;
    NickSV::Chat::ClientInfo<CharT> ClientInfo1, ClientInfo2;
    std::basic_string<CharT> nick = NickSV::Chat::basic_string_cast<CharT>("Hello there, I am a test nickname!");
    ClientInfo1.m_sNick = nick;
    auto serializer = ClientInfo1.GetSerializer();
    std::string str = serializer.ToString();
    ClientInfo2 = NickSV::Chat::ClientInfoSerializer<CharT>::MakeFromString(str);
    IF_RETURN(ClientInfo2 != ClientInfo1, stage);
    stage++;

    str = str.substr(0, str.size()-1);
    ClientInfo2 = NickSV::Chat::ClientInfoSerializer<CharT>::MakeFromString(str);
    IF_RETURN(ClientInfo2.IsValid(), stage);
    stage++;

    return 0;
}


int main()
{
    int ntest_failed = 0;

    RESULT(client_info_test<char>(), ntest_failed);
    RESULT(client_info_test<wchar_t>(), ntest_failed);
    RESULT(client_info_test<char16_t>(), ntest_failed);
    RESULT(client_info_test<char32_t>(), ntest_failed);

    RESULT(client_info_serializer_test<char>(), ntest_failed);
    RESULT(client_info_serializer_test<wchar_t>(), ntest_failed);
    RESULT(client_info_serializer_test<char16_t>(), ntest_failed);
    RESULT(client_info_serializer_test<char32_t>(), ntest_failed);

    std::cout << '\n' << ntest_failed << " tests failed" << std::endl;
    

    return 0;
}