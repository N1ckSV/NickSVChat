
#ifndef _NICKSV_CHAT_UTILS
#define _NICKSV_CHAT_UTILS
#pragma once



#include <string>
#include <memory>
#include <mutex>
#include <queue>
#include <future>

#include <steam/isteamnetworkingutils.h>

#include "NickSV/Tools/TypeTraits.h"
#include "NickSV/Tools/Memory.h"

#include "NickSV/Chat/Types.h"


namespace NickSV {
namespace Chat {



/*
 Versions converting
*/
constexpr Version_t ConvertVersions(Version_t major, Version_t minor, Version_t patch, Version_t tweak)
{
    CHAT_ASSERT(IS_VERSION_VALID(major, minor, patch, tweak), VERSION_PROTOCOL_ERROR_MESSAGE);
    return (major << 24) + (minor << 16) + (patch << 8) + tweak;
}

constexpr Version_t ConvertVersionToMajor(Version_t fullVersion) { return (fullVersion      ) >> 24; }
constexpr Version_t ConvertVersionToMinor(Version_t fullVersion) { return (fullVersion << 8 ) >> 24; }
constexpr Version_t ConvertVersionToPatch(Version_t fullVersion) { return (fullVersion << 16) >> 24; }
constexpr Version_t ConvertVersionToTweak(Version_t fullVersion) { return (fullVersion << 24) >> 24; }


/*
Serializable type-traits.
See also is_serializable_ERROR_MESSAGE.
*/
template<typename ToSerialize>
struct is_serializable : std::is_base_of<ISerializable, ToSerialize> {};


/*
Parsable type-traits.
See also is_parsable_ERROR_MESSAGE.
*/
template<typename ToParse>
struct is_parsable : std::is_default_constructible<ToParse> {};


/*
Same as Parser<ToParse>::FromString(...) but creates 
ToParse object.
If str is bad (Unparsable), return value is ToParse()
*/
template<typename ToParse>
ToParse MakeFromString(const std::string& str)
{
    static_assert(is_parsable<ToParse>::value, is_parsable_ERROR_MESSAGE);
    auto parser = Parser<ToParse>();
    if(std::distance(str.cbegin(), parser.FromString(str)) <= 0)
        return ToParse();

    return parser.GetObject();
}

struct NICKSVCHAT_API ChatIPAddr : public SteamNetworkingIPAddr
{
    ChatIPAddr();
    std::string ToString();
};






#define CASE_PRINT_ENUM(enum_val) case enum_val : os << #enum_val; break;

template<class CharT>
std::basic_ostream<CharT>& operator<<(std::basic_ostream<CharT>& os, EState state)
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
std::basic_ostream<CharT>& operator<<(std::basic_ostream<CharT>& os, EResult res)
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
std::basic_ostream<CharT>& operator<<(std::basic_ostream<CharT>& os, ERequestType type)
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
std::basic_ostream<CharT>& operator<<(std::basic_ostream<CharT>& os, ESteamNetworkingConnectionState steamState)
{
    os << static_cast<int>(steamState) << ":" << "SteamConState_";
    switch (steamState) {
    case k_ESteamNetworkingConnectionState_None: os << "None"; break;
    case k_ESteamNetworkingConnectionState_ClosedByPeer: os << "ClosedByPeer"; break;
    case k_ESteamNetworkingConnectionState_Connecting: os << "Connecting"; break;
    case k_ESteamNetworkingConnectionState_Connected: os << "Connected"; break;
    case k_ESteamNetworkingConnectionState_FindingRoute: os << "FindingRoute"; break;
    case k_ESteamNetworkingConnectionState_ProblemDetectedLocally: os << "LocalProblem"; break;
    default: os << "Unimplemented"; break;
    }
    return os;
}




template<class ResultT, class Other, class... Others>
constexpr auto ResultIsOneOf(ResultT result, Other other, Others... others) 
-> typename std::enable_if<
    std::is_same<decltype(static_cast<EResult>(result)), EResult>::value &&
    std::is_same<decltype(static_cast<EResult>(other )), EResult>::value ,
bool>::type
{
    return  (static_cast<EResult>(result) == static_cast<EResult>(other)) || ResultIsOneOf(result, others...);
}

template<class ResultT, class Other>
constexpr auto ResultIsOneOf(ResultT result, Other other) 
-> typename std::enable_if<
    std::is_same<decltype(static_cast<EResult>(result)), EResult>::value &&
    std::is_same<decltype(static_cast<EResult>(other )), EResult>::value ,
bool>::type
{
    return  static_cast<EResult>(result) == static_cast<EResult>(other);
}

/**
 * @returns true if UserID_t is reserved by Lib and false otherwise
 */
constexpr inline bool IsLibReservedID(UserID_t id)
{
	return id < Constant::LibReservedUserIDs;
}





template<class Arg>
auto ParseSeries(std::string::const_iterator begin, 
              std::string::const_iterator end, 
              Arg& arg)
-> std::enable_if_t<Tools::is_type_complete<Parser<Arg>>::value, std::string::const_iterator>
{
    static_assert(std::is_copy_assignable<Arg>::value || std::is_move_assignable<Arg>::value,
         "Cannot ParseSeries given type Arg, it is unassignable");

    auto parser = Parser<Arg>();
    auto iter = parser.FromString(begin, end);
    if(std::distance(begin, iter) <= 0)
        return begin; //BAD INPUT. Parsed size doesnt match with input size
    
    std::swap(arg, parser.GetObject());
    return iter;
}

template<class Arg, class... Args>
auto ParseSeries(std::string::const_iterator begin, 
              std::string::const_iterator end, 
              Arg& arg, 
              Args&... args)
-> std::enable_if_t<Tools::is_type_complete<Parser<Arg>>::value, std::string::const_iterator>
{
    static_assert(std::is_copy_assignable<Arg>::value || std::is_move_assignable<Arg>::value,
         "Cannot ParseSeries given type Arg, it is unassignable");

    auto parser = Parser<Arg>();
    auto iter = parser.FromString(begin, end);
    if(std::distance(begin, iter) <= 0)
        return begin; //BAD INPUT. Parsed size doesnt match with input size
    
    std::swap(arg, parser.GetObject());
    auto newIter = ParseSeries(iter, end, args...);
    if(std::distance(iter, newIter) <= 0)
        return begin;
    return newIter;
}














template<class Arg>
auto ParseSeries(std::string::const_iterator begin, 
              std::string::const_iterator end, 
              Arg& arg)
-> std::enable_if_t<
    !Tools::is_type_complete<Parser<Arg>>::value && 
    (std::is_fundamental<Arg>::value || std::is_enum<Arg>::value), 
    std::string::const_iterator>
{
    static_assert(std::is_copy_assignable<Arg>::value || std::is_move_assignable<Arg>::value,
         "Cannot ParseSeries given type Arg, it is unassignable");

    if(std::distance(begin, end) < static_cast<std::ptrdiff_t>(sizeof(Arg)))
        return begin; //BAD INPUT. Range size has to be atleast size of Arg bytes long
    
    Transfer<Arg> ArgUnion;
    std::copy(begin, begin + sizeof(Arg), ArgUnion.CharArr);
    arg = ArgUnion.Base;
    return begin + sizeof(Arg);
}


template<class Arg, class... Args>
auto ParseSeries(std::string::const_iterator begin, 
              std::string::const_iterator end, 
              Arg& arg, 
              Args&... args)
-> std::enable_if_t<
    !Tools::is_type_complete<Parser<Arg>>::value && 
    (std::is_fundamental<Arg>::value || std::is_enum<Arg>::value), 
    std::string::const_iterator>
{
    static_assert(std::is_copy_assignable<Arg>::value || std::is_move_assignable<Arg>::value,
         "Cannot ParseSeries given type Arg, it is unassignable");

    if(std::distance(begin, end) <= static_cast<std::ptrdiff_t>(sizeof(Arg)))
        return begin; //BAD INPUT. Range size has to be atleast size of Arg bytes long
    
    Transfer<Arg> ArgUnion;
    std::copy(begin, begin + sizeof(Arg), ArgUnion.CharArr);
    arg = ArgUnion.Base;
    auto iter = begin + sizeof(Arg);
    auto newIter = ParseSeries(iter, end, args...);
    if(std::distance(iter, newIter) <= 0)
        return begin;
    return newIter;
}



template<class T>
class NICKSVCHAT_API SafeQueue final
{
public:
    using ValueType = T;
    using BaseType = std::queue<ValueType>;
    using MutexType = std::mutex;

    SafeQueue() = default;
    DECLARE_RULE_OF_5_DELETE(SafeQueue);

    /**
     * @returns old state whether Push was disabled or alredy enabled
     */
    bool EnablePush()
    {
        std::lock_guard<std::mutex> guard(m_mtx);
        bool old = m_bPushEnabled;
        m_bPushEnabled = true;
        return old;
    }

    /**
     * @returns old state whether Push was enabled or alredy disabled
     */
    bool DisablePush()
    {
        std::lock_guard<std::mutex> guard(m_mtx);
        bool old = m_bPushEnabled;
        m_bPushEnabled = false;
        return old;
    }


    inline BaseType& GetBase() noexcept
    { return m_queue; }

    inline const BaseType& GetBase() const noexcept
    { return m_queue; }

    inline const BaseType& GetConstBase() const noexcept
    { return m_queue; }

    inline MutexType& GetMutex() const noexcept
    { return m_mtx; }

    /**
     * @brief Pushing value if
     * push is allowed
     * 
     * @returns result is whether value is pushed
     */
    bool Push(const ValueType& value) noexcept(false)
    {
        std::lock_guard<std::mutex> guard(m_mtx);
        if(m_bPushEnabled)
            m_queue.push(value);
        return m_bPushEnabled;
    }

    /**
     * @brief Pushing value if
     * push is allowed
     * 
     * @returns result is whether value is pushed
     */
    bool Push(ValueType&& value) noexcept(false)
    {
        std::lock_guard<std::mutex> guard(m_mtx);
        if(m_bPushEnabled)
            m_queue.push(std::move(value));
        return m_bPushEnabled;
    }

    ValueType Pop() noexcept(false)
    {
        std::lock_guard<std::mutex> guard(m_mtx);
        auto tmp = std::move(m_queue.front());
        m_queue.pop();
        return tmp;
    }

    bool IsEmpty() const
    {
        std::lock_guard<std::mutex> guard(m_mtx);
        return m_queue.empty();
    }

    size_t Size() const
    {
        std::lock_guard<std::mutex> guard(m_mtx);
        return m_queue.size();
    }

    inline void Lock() const
    { m_mtx.lock(); }

    inline void Unlock() const
    { m_mtx.unlock(); }

    inline bool TryLock() const
    { return m_mtx.try_lock(); }

private:
    BaseType m_queue;
    mutable MutexType m_mtx;
    bool volatile m_bPushEnabled = true;
};




class NICKSVCHAT_API TaskInfo
{
public:
    DECLARE_COPY_DELETE(TaskInfo);
    DECLARE_MOVE_DEFAULT(TaskInfo, NOTHING);
    TaskInfo() = default;
    TaskInfo(EResult, std::future<EResult>&&);

    std::future<EResult>& GetFuture();
    EResult               GetInitialResult() const;
    EResult               GetFutureResult() noexcept(false);

    void Wait() const;
    void SetInitialResult(EResult result) noexcept;
    void SetFuture(std::future<EResult>&& result) noexcept;

private:
    EResult m_eInitialResult;
    std::future<EResult> m_futureResult;
};













}}  /*END OF NAMESPACES*/




#endif // _NICKSV_CHAT_UTILS