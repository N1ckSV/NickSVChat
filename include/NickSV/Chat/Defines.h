
#ifndef _NICKSV_CHAT_DEFINES
#define _NICKSV_CHAT_DEFINES
#pragma once


#include "ChatConfig.h"

//ASSERT STUFF
#ifdef CHAT_DEBUG
    #include <assert.h>
    #define CHAT_ASSERT(exp, msg) assert((exp) && (msg))
#else
    #define CHAT_ASSERT(exp, msg) (void(0))
#endif



//GameNetworkingSockets for opensource
#ifndef STEAMNETWORKINGSOCKETS_OPENSOURCE
#define STEAMNETWORKINGSOCKETS_OPENSOURCE
#endif



//Just macroed it for convenience :-)
#ifndef CHAT_NODISCARD
#define CHAT_NODISCARD [[nodiscard]]
#endif

#define VERSION_PROTOCOL_ERROR_MESSAGE "Version protocol rule violated: Each NICKSVCHAT_VERSION_<ver-name> should be between 0 to 255."

#define IS_VERSION_VALID(major, minor, patch, tweak)\
(((major) < 256) && ((minor) < 256) && ((patch) < 256) && ((tweak) < 256) && ((major) >= 0 ) && ((minor) >= 0 ) && ((patch) >= 0 ) && ((tweak) >= 0 ))

#if !IS_VERSION_VALID(NICKSVCHAT_VERSION_MAJOR, NICKSVCHAT_VERSION_MINOR, NICKSVCHAT_VERSION_PATCH, 0)
    #error Version protocol rule violated: Each NICKSVCHAT_VERSION_<ver-name> should be between 0 to 255.
#endif


// STRING STUFF
//REDEFINE YOUR CHARACTER-LIKE TYPE HERE
#if defined (USE_CHAR)
    typedef char TCHAR;
    #define _T(a) a
    #define sout std::cout
#elif defined (USE_CHAR16)
    typedef char16_t TCHAR;
    #define _T(a) u##a
    #define sout
#elif defined (USE_CHAR32)
    typedef char32_t TCHAR;
    #define _T(a) U##a
    #define sout
#elif defined (USE_WCHAR)
    typedef wchar_t TCHAR;
    #define _T(a) L##a
    #define sout std::wcout
#else
    #error Something went wrong. No character type is used
#endif
#define TEXT(x) _T(x)










#endif // _NICKSV_CHAT_DEFINES