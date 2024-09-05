
#ifndef _NICKSV_CHAT_DEFINITIONS
#define _NICKSV_CHAT_DEFINITIONS
#pragma once


#include "ChatConfig.h"

#include "NickSV/Tools/Definitions.h"

//ASSERT STUFF (EXPECT is non-fatal one)
#if !defined(NDEBUG) || defined(_DEBUG) 
    #include <assert.h>
    #define CHAT_ASSERT(exp, msg) assert((exp) && (msg))
    #define CHAT_EXPECT(exp, msg) do { if (!(exp)){fprintf(stderr, "%s failed in %s:%d\n", #exp, __FILE__, __LINE__);}} while(0)
    #define CHAT_DEBUG(...) (__VA_ARGS__)
    #define _CHAT_DEBUG
#else
    #define CHAT_ASSERT(exp, msg) (void(0))
    #define CHAT_EXPECT(exp, msg) (void(0))
    #define CHAT_DEBUG(...)       (void(0))
#endif



#if defined( NICKSVCHAT_STATIC_LINK )
	#define NICKSVCHAT_API
	#define NICKSVCHAT_API_EXPORT
#elif defined( NICKSVCHAT_FOREXPORT ) && defined( _WIN32) && defined( _MSC_VER )
    // I dunno __declspec( dllexport/import )                           
    // is for MSVC in general
    // or for MSVC with Windows only 
	#define NICKSVCHAT_API  __declspec( dllexport )
	#define NICKSVCHAT_API_EXPORT __declspec( dllexport )
#elif defined( NICKSVCHAT_FOREXPORT ) && !defined( _WIN32 )
	#define NICKSVCHAT_API __attribute__((visibility("default")))
	#define NICKSVCHAT_API_EXPORT
#elif !defined( NICKSVCHAT_FOREXPORT ) && defined( _MSC_VER ) 
	#define NICKSVCHAT_API __declspec( dllimport )
	#define NICKSVCHAT_API_EXPORT __declspec( dllexport )
#else 
	#define NICKSVCHAT_API
	#define NICKSVCHAT_API_EXPORT
#endif

#define NICKSVCHAT_INTERFACE extern "C" NICKSVCHAT_API

//Default port FIXME DELETE IT
#define DEFAULT_PORT 27020



#define VERSION_PROTOCOL_ERROR_MESSAGE "Version protocol rule violated: Each NICKSVCHAT_VERSION_<ver-name> should be between 0 to 255."

#define IS_VERSION_VALID(major, minor, patch, tweak)\
((((major) % 256) == (major)) && (((minor) % 256) == (minor))  && (((patch) % 256) == (patch)))

#if !IS_VERSION_VALID(NICKSVCHAT_VERSION_MAJOR, NICKSVCHAT_VERSION_MINOR, NICKSVCHAT_VERSION_PATCH, 0)
    #error Version protocol rule violated: Each NICKSVCHAT_VERSION_<ver-name> should be between 0 to 255.
#endif

#ifndef MAX_NICKNAME_SIZE
#define MAX_NICKNAME_SIZE 50
#endif

// STRING STUFF
//REDEFINE YOUR CHARACTER-LIKE TYPE HERE
#if defined (USE_CHAR)
    using CHAT_CHAR = char;
    #define _T(a) a
    #define sout std::cout
#elif defined (USE_CHAR16)
    using CHAT_CHAR = char16_t;
    #define _T(a) u##a
    #define sout
#elif defined (USE_CHAR32)
    using CHAT_CHAR = char32_t;
    #define _T(a) U##a
    #define sout
#elif defined (USE_WCHAR)
    using CHAT_CHAR = wchar_t;
    #define _T(a) L##a
    #define sout std::wcout
#else
    #error Something went wrong. No character type is used
#endif


#define TEXT(text, type) NickSV_TEXT(text, type)


//Send flags
#define SF_SEND_TO_ONE    0
#define SF_SEND_TO_ALL    1
#define SF_SEND_TO_ACTIVE 2


//ERROR MESSAGES
#define const_class_ERROR_MESSAGE "You are using ConstClass without the const specifier, so this function has no effect."
#define is_char_ERROR_MESSAGE "Type must be character-like type (char, wchar_t etc). See is_char type trait declaration."
#define is_serializable_ERROR_MESSAGE "Type must be serializable. See requirements in is_serializable definition."
#define is_parsable_ERROR_MESSAGE "Type must be parsable. See requirements in is_parsable definition."
#define something_went_wrong_ERROR_MESSAGE "Something went wrong and probably the above code is broken."
#define invalid_range_size_ERROR_MESSAGE "Size of given range is invalid."
#define serialize_request_ERROR_MESSAGE "Request is 'Unknown' request so you are not supposed to serialize it to string, but parse."













#endif // _NICKSV_CHAT_DEFINITIONS