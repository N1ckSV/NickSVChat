
#include "NickSV/Chat/Defines.h"
#include "NickSV/Chat/Types.h"
#include "NickSV/Chat/Utils.h"
#include "NickSV/Chat/ClientInfo.h"
#include "NickSV/Chat/Serializers.h"
#include <iostream>
#include <iomanip>
#include <exception>


#define SETW_VAL 40

#define IF_RETURN(exp, ret) if(exp) { return (ret); }
#define IFN_RETURN(exp, ret) if(!exp) { return (ret); }

#define RESULT(stage, failed)                                              \
    try {                                                                  \
        std::cout << std::setw(SETW_VAL) << std::string(#stage) << ": ";   \
        if(!stage) {                                                       \
            std::cout << std::string("PASSED") << std::endl;               \
        }                                                                  \
        else {                                                             \
            failed++;                                                      \
            std::cout  << std::string("FAILED on stage ")                  \
            << stage << std::endl;                                         \
        }                                                                  \
    } catch (const std::exception& ex) {                                   \
        std::cout << "Exception thrown - " << ex.what() << std::endl;      \
    }                                                                      \



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

int versio_conversation_test()
{
    int stage = 1;
    NickSV::Chat::Version_t ver = NickSV::Chat::ConvertVersions(NICKSVCHAT_VERSION_MAJOR, NICKSVCHAT_VERSION_MINOR, NICKSVCHAT_VERSION_PATCH, 0);
    IF_RETURN(NickSV::Chat::ConvertVersionToMajor(ver) != NICKSVCHAT_VERSION_MAJOR, stage)
    stage++;
    IF_RETURN(NickSV::Chat::ConvertVersionToMinor(ver) != NICKSVCHAT_VERSION_MINOR, stage)
    stage++;
    IF_RETURN(NickSV::Chat::ConvertVersionToPatch(ver) != NICKSVCHAT_VERSION_PATCH, stage)
    stage++;
    IF_RETURN(NickSV::Chat::ConvertVersionToTweak(ver) != 0, stage)
    stage++;

    return 0;
}


int main(int arc, const char ** argv)
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

    RESULT(versio_conversation_test(), ntest_failed);

    std::cout << '\n' << ntest_failed << " tests failed" << std::endl;
    

    return ntest_failed;
}