
#ifndef _NICKSV_CHAT_DEFINES
#define _NICKSV_CHAT_DEFINES
#pragma once


#include "ChatConfig.h"

//ASSERT STUFF (EXPECT is non-fatal one)
#if defined(CHAT_DEBUG)
    #include <assert.h>
    #define CHAT_ASSERT(exp, msg) assert((exp) && (msg))
    #define CHAT_EXPECT(exp, msg) do { if (!(exp)){fprintf(stderr, "%s failed in %s:%d\n", #exp, __FILE__, __LINE__);}} while(0)
#else
    #define CHAT_ASSERT(exp, msg) (void(0))
    #define CHAT_EXPECT(exp, msg) (void(0))
#endif



//GameNetworkingSockets for opensource
#ifndef STEAMNETWORKINGSOCKETS_OPENSOURCE
#define STEAMNETWORKINGSOCKETS_OPENSOURCE
#endif


#if defined( NICKSVCHAT_STATIC_LINK )
	#define NICKSVCHAT_INTERFACE extern "C"
#elif defined( NICKSVCHAT_FOREXPORT )
	#if defined( _WIN32 )
		#define NICKSVCHAT_INTERFACE extern "C" __declspec( dllexport )
	#else
		#define NICKSVCHAT_INTERFACE extern "C" __attribute__((visibility("default")))
	#endif
#else
	#ifdef _WIN32
		#define NICKSVCHAT_INTERFACE extern "C" __declspec( dllimport )
	#else
		#define NICKSVCHAT_INTERFACE extern "C"
	#endif
#endif

//Default port
#define DEFAULT_PORT 27020

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

#ifndef MAX_NICKNAME_SIZE
#define MAX_NICKNAME_SIZE 50
#endif

// STRING STUFF
//REDEFINE YOUR CHARACTER-LIKE TYPE HERE
#if defined (USE_CHAR)
    typedef char CHAT_CHAR;
    #define _T(a) a
    #define sout std::cout
#elif defined (USE_CHAR16)
    typedef char16_t CHAT_CHAR;
    #define _T(a) u##a
    #define sout
#elif defined (USE_CHAR32)
    typedef char32_t CHAT_CHAR;
    #define _T(a) U##a
    #define sout
#elif defined (USE_WCHAR)
    typedef wchar_t CHAT_CHAR;
    #define _T(a) L##a
    #define sout std::wcout
#else
    #error Something went wrong. No character type is used
#endif
#define TEXT(x) _T(x)


//Send flags
#define SF_SEND_TO_ALL 0
#define SF_SEND_TO_ONE 1


//ERROR MESSAGES
#define const_class_ERROR_MESSAGE "You are using ConstClass without the const specifier, so this function has no effect."
#define is_char_ERROR_MESSAGE "Type must be character-like type (char, wchar_t etc). See is_char type trait declaration."
#define is_serializable_ERROR_MESSAGE "Type must be serializable. See requirements in is_serializable definition."
#define is_parsable_ERROR_MESSAGE "Type must be parsable. See requirements in is_parsable definition."
#define something_went_wrong_ERROR_MESSAGE "Something went wrong and probably the above code is broken."
#define invalid_range_size_ERROR_MESSAGE "Size of given range is invalid."
#define serialize_request_ERROR_MESSAGE "Request is 'Unknown' request so you are not supposed to serialize it to string, but parse."













#endif // _NICKSV_CHAT_DEFINES