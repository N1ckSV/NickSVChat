
#include <ostream>

#include "NickSV/Tools/Utils.h"

#include "NickSV/Chat/Utils.h"


namespace NickSV {
namespace Chat {



//-----------------------------------------------------------------------------------
// Impementation of ChatIPAddr
//-----------------------------------------------------------------------------------
ChatIPAddr::ChatIPAddr()
{
    ParseString("127.0.0.1");
    m_port = DEFAULT_PORT;
}

std::string ChatIPAddr::ToString()
{
	std::string str(SteamNetworkingIPAddr::k_cchMaxString, '0');
    SteamNetworkingIPAddr::ToString(Tools::MutableStringData(str), SteamNetworkingIPAddr::k_cchMaxString, true);
	return str;
}
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------






//-----------------------------------------------------------------------------------
// Impementation of RequestInfo
//-----------------------------------------------------------------------------------
RequestInfo::RequestInfo() : userID(0), sendFlags(0), extraInfo{0} {};
RequestInfo::RequestInfo(UserID_t _id) : userID(_id), sendFlags(0), extraInfo{0} {};
RequestInfo::RequestInfo(UserID_t _id, uint64_t flags) : userID(_id), sendFlags(flags), extraInfo{0} {};
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------





bool IsValidUTF8String(const std::string& str)
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




//-----------------------------------------------------------------------------------
// Impementation of TaskInfo
//-----------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------



}}  /*END OF NAMESPACES*/