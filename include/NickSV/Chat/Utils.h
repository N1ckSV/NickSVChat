

#ifndef _NICKSV_CHAT_UTILS
#define _NICKSV_CHAT_UTILS
#pragma once


#include <type_traits>
#include <string>
#include <cstring>


#include "NickSV/Chat/Defines.h"
#include "NickSV/Chat/Types.h"


namespace NickSV::Chat {

/*
Character-like type-traits
*/
template<typename CharT = char>
struct is_char : std::integral_constant<bool,
                     std::is_same<CharT, char>::value     ||
                     std::is_same<CharT, char16_t>::value ||
                     std::is_same<CharT, char32_t>::value ||
                     std::is_same<CharT, wchar_t>::value>
{
#define is_char_ERROR_MESSAGE "Type must be character-like type (char, wchar_t etc). See is_char type trait declaration."
};

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

constexpr Version_t ConvertVersions(Version_t major, Version_t minor = 0U, Version_t patch = 0U, Version_t tweak = 0U)
{
    CHAT_ASSERT(IS_VERSION_VALID(major, minor, patch, tweak), VERSION_PROTOCOL_ERROR_MESSAGE);
    return (major << 24) + (minor << 16) + (patch << 8) + tweak;
}
constexpr Version_t ConvertVersionToMajor(Version_t fullVersion) { return (fullVersion      ) >> 24; }
constexpr Version_t ConvertVersionToMinor(Version_t fullVersion) { return (fullVersion << 8 ) >> 24; }
constexpr Version_t ConvertVersionToPatch(Version_t fullVersion) { return (fullVersion << 16) >> 24; }
constexpr Version_t ConvertVersionToTweak(Version_t fullVersion) { return (fullVersion << 24) >> 24; }


} /*END OF NAMESPACES*/



#endif // _NICKSV_CHAT_UTILS