
#include <ostream>

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




}}  /*END OF NAMESPACES*/