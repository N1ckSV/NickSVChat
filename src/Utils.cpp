
#include <ostream>

#include <steam/isteamnetworkingutils.h>

#include "NickSV/Chat/Utils.h"

namespace NickSV {
namespace Chat {



ChatIPAddr::ChatIPAddr()
{
    ParseString("127.0.0.1");
    m_port = DEFAULT_PORT;
}

std::string ChatIPAddr::ToString()
{
	std::string str(SteamNetworkingIPAddr::k_cchMaxString, '0');
    SteamNetworkingIPAddr::ToString(str.data(), SteamNetworkingIPAddr::k_cchMaxString, true);
	return str;
}



#define CASE_PRINT_ENUM(enum_val) case enum_val : os << #enum_val; break;

template<class CharT>
std::basic_ostream<CharT>& operator<<(std::basic_ostream<CharT> & os, EState state)
{
	static_assert(EState::Size == EState(6U), "Printing of new EState value was not added");
	os << static_cast<uint32_t>(state) << ":";
	switch (state) {
	CASE_PRINT_ENUM(EState::Active);
	CASE_PRINT_ENUM(EState::Busy);
	CASE_PRINT_ENUM(EState::Free);
	CASE_PRINT_ENUM(EState::Invalid);
	CASE_PRINT_ENUM(EState::Reserved);
	CASE_PRINT_ENUM(EState::Unauthorized);
	default: os << "EState::Unimplemented"; break;
	}
	return os;
}

template<class CharT>
std::basic_ostream<CharT>& operator<<(std::basic_ostream<CharT> & os, EResult res)
{
	static_assert(EResult::Size == EResult(12U), "Printing of new EResult value was not added");
	os << static_cast<uint32_t>(res) << ":";
    switch (res) {
    CASE_PRINT_ENUM(EResult::AlreadyRunning);
    CASE_PRINT_ENUM(EResult::ClosedSocket);
    CASE_PRINT_ENUM(EResult::ClosedConnection);
    CASE_PRINT_ENUM(EResult::Error);
    CASE_PRINT_ENUM(EResult::TimeoutExpired);
    CASE_PRINT_ENUM(EResult::InvalidConnection);
    CASE_PRINT_ENUM(EResult::InvalidParam);
    CASE_PRINT_ENUM(EResult::InvalidRequest);
    CASE_PRINT_ENUM(EResult::InvalidSize);
    CASE_PRINT_ENUM(EResult::NoAction);
    CASE_PRINT_ENUM(EResult::Overflow);
    CASE_PRINT_ENUM(EResult::Success);
    CASE_PRINT_ENUM(EResult::Size);
    default: os << "EResult::Unimplemented"; break;
    }
    return os;
}

template<class CharT>
std::basic_ostream<CharT>& operator<<(std::basic_ostream<CharT> & os, ERequestType type)
{
	static_assert(ERequestType::Size == ERequestType(3U), "Printing of new ERequestType value was not added");
	os << static_cast<uint32_t>(type) << ":";
  	switch (type) {
  	CASE_PRINT_ENUM(ERequestType::ClientInfo);
  	CASE_PRINT_ENUM(ERequestType::Message);
  	CASE_PRINT_ENUM(ERequestType::Unknown);
  	default: os << "ERequestType::Unimplemented"; break;
  	}
  	return os;
}


template<class CharT>
std::basic_ostream<CharT>& operator<<(std::basic_ostream<CharT> & os, ESteamNetworkingConnectionState steamState)
{
	os << static_cast<int>(steamState) << ":" << "SteamConState_";
  	switch (steamState) {
  	case k_ESteamNetworkingConnectionState_None  				  : os << "None" 		 ; break;
  	case k_ESteamNetworkingConnectionState_ClosedByPeer           : os << "ClosedByPeer" ; break;
  	case k_ESteamNetworkingConnectionState_Connecting 			  : os << "Connecting"   ; break;
  	case k_ESteamNetworkingConnectionState_Connected 			  : os << "Connected"    ; break;
  	case k_ESteamNetworkingConnectionState_FindingRoute 		  : os << "FindingRoute" ; break;
  	case k_ESteamNetworkingConnectionState_ProblemDetectedLocally : os << "LocalProblem" ; break;
  	default														  : os << "Unimplemented"; break;
  	}
  	return os;
}



template std::basic_ostream<char>& operator<<<char>(std::basic_ostream<char> & os, EState state);
template std::basic_ostream<wchar_t>& operator<<<wchar_t>(std::basic_ostream<wchar_t> & os, EState state);
template std::basic_ostream<char16_t>& operator<<<char16_t>(std::basic_ostream<char16_t> & os, EState state);
template std::basic_ostream<char32_t>& operator<<<char32_t>(std::basic_ostream<char32_t> & os, EState state);

template std::basic_ostream<char>& operator<<<char>(std::basic_ostream<char> & os, EResult res);
template std::basic_ostream<wchar_t>& operator<<<wchar_t>(std::basic_ostream<wchar_t> & os, EResult res);
template std::basic_ostream<char16_t>& operator<<<char16_t>(std::basic_ostream<char16_t> & os, EResult res);
template std::basic_ostream<char32_t>& operator<<<char32_t>(std::basic_ostream<char32_t> & os, EResult res);

template std::basic_ostream<char>& operator<<<char>(std::basic_ostream<char> & os, ERequestType type);
template std::basic_ostream<wchar_t>& operator<<<wchar_t>(std::basic_ostream<wchar_t> & os, ERequestType type);
template std::basic_ostream<char16_t>& operator<<<char16_t>(std::basic_ostream<char16_t> & os, ERequestType type);
template std::basic_ostream<char32_t>& operator<<<char32_t>(std::basic_ostream<char32_t> & os, ERequestType type);

template std::basic_ostream<char>& operator<<<char>(std::basic_ostream<char> & os, ESteamNetworkingConnectionState state);
template std::basic_ostream<wchar_t>& operator<<<wchar_t>(std::basic_ostream<wchar_t> & os, ESteamNetworkingConnectionState state);
template std::basic_ostream<char16_t>& operator<<<char16_t>(std::basic_ostream<char16_t> & os, ESteamNetworkingConnectionState state);
template std::basic_ostream<char32_t>& operator<<<char32_t>(std::basic_ostream<char32_t> & os, ESteamNetworkingConnectionState state);




TaskInfo::TaskInfo(EResult initialRes, std::future<EResult>&& sendResult)
    : m_eInitialResult(initialRes), m_futureResult(std::move(sendResult)) {}
 
std::future<EResult>& TaskInfo::GetFuture()
{ return m_futureResult; }

EResult TaskInfo::GetInitialResult() const
{ return m_eInitialResult; }

EResult TaskInfo::GetFutureResult() noexcept(false)
{ return m_futureResult.get(); }

void TaskInfo::Wait() const
{ return m_futureResult.wait(); }

void TaskInfo::SetInitialResult(EResult result) noexcept
{ m_eInitialResult = result; }

void TaskInfo::SetFuture(std::future<EResult>&& resultFuture) noexcept
{ m_futureResult = std::move(resultFuture); }

//ChatIPAddr::operator SteamNetworkingIPAddr() const
//{
//    SteamNetworkingIPAddr addr;
//    addr.SetIPv6(m_ipv6, m_port);
//    return addr;
//}



}}  /*END OF NAMESPACES*/