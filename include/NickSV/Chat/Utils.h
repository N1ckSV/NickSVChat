
#ifndef _NICKSV_CHAT_UTILS
#define _NICKSV_CHAT_UTILS
#pragma once



#include <string>
#include <memory>
#include <mutex>
#include <queue>
#include <future>


#include "NickSV/Chat/Types.h"

#include <steam/steamnetworkingsockets.h>


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
    if(parser.FromString(str) <= str.begin())
        return ToParse();

    return *parser.GetObject();
}

struct ChatIPAddr : public SteamNetworkingIPAddr
{
    ChatIPAddr();
    std::string ToString();
};




template<class CharT>
std::basic_ostream<CharT>& operator<<(std::basic_ostream<CharT> & os, EState state);

template<class CharT>
std::basic_ostream<CharT>& operator<<(std::basic_ostream<CharT> & os, ERequestType state);

template<class CharT>
std::basic_ostream<CharT>& operator<<(std::basic_ostream<CharT> & os, EResult state);

template<class CharT>
std::basic_ostream<CharT>& operator<<(std::basic_ostream<CharT> & os, ESteamNetworkingConnectionState steamState);






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




template<class T>
class SafeQueue final
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




class TaskInfo
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