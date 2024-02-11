
#ifndef _CHAT_DEFINES
#define _CHAT_DEFINES


//ASSERT STUFF
#ifdef CHAT_DEBUG
    #include <assert.h>
    #define CHAT_ASSERT(exp, msg) assert(((void)msg, exp))
#else
    #define CHAT_ASSERT(exp, msg) (void(0))
#endif



//VERSION STUFF
#define NICKSVCHAT_VERSION 1           /// FIXME: MAKE VERSION PROTOCOL



//GameNetworkingSockets for opensource
#ifndef STEAMNETWORKINGSOCKETS_OPENSOURCE
#define STEAMNETWORKINGSOCKETS_OPENSOURCE
#endif



//Just macroed it for convenience :-)
#ifndef CHAT_NODISCARD
#define CHAT_NODISCARD [[nodiscard]]
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










#endif // _CHAT_DEFINES