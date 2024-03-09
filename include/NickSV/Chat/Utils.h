
#ifndef _NICKSV_CHAT_UTILS
#define _NICKSV_CHAT_UTILS
#pragma once


#include <type_traits>
#include <string>
#include <cstring>
#include <memory>

#include "NickSV/Chat/Types.h"

#include <steam/steamnetworkingsockets.h>


namespace NickSV::Chat {


/*
Character-like type-traits.
See is_char_ERROR_MESSAGE
*/

template<typename CharT>
struct is_char_helper : std::false_type {};
template<>
struct is_char_helper<char> : std::true_type {};
template<>
struct is_char_helper<wchar_t> : std::true_type {};
template<>
struct is_char_helper<char16_t> : std::true_type {};
template<>
struct is_char_helper<char32_t> : std::true_type {};

template<typename CharT>
struct is_char : is_char_helper<typename std::remove_cv<CharT>::type> {};

/*
 Runtime conversation from [char const *] to [std::basic_string<CharT>]
*/
template <typename CharT>
std::basic_string<CharT> basic_string_cast(char const *pcszToConvert)
{
    std::basic_string<CharT> bstr;
    size_t sz = strlen(pcszToConvert);
    bstr.resize(sz);
    std::copy(pcszToConvert, pcszToConvert + sz, bstr.begin());
    return bstr; 
}

template <>
std::string inline basic_string_cast<char>(char const * pcszToConvert) { return std::string(pcszToConvert); };


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
};


} /*END OF NAMESPACES*/




#endif // _NICKSV_CHAT_UTILS