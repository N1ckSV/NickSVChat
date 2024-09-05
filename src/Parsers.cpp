
#include <cstring>
#include <memory>

#include "NickSV/Chat/Utils.h"

#include "Parsers/BStringParser.hpp"
#include "Parsers/ClientInfoParser.hpp"
#include "Parsers/MessageParser.hpp"
#include "Parsers/RequestParser.hpp"
#include "Parsers/ClientInfoRequestParser.hpp"
#include "Parsers/MessageRequestParser.hpp"



namespace NickSV {
namespace Chat {


template class NICKSVCHAT_API_EXPORT Parser<std::basic_string<char>>;
template class NICKSVCHAT_API_EXPORT Parser<std::basic_string<wchar_t>>;
template class NICKSVCHAT_API_EXPORT Parser<std::basic_string<char16_t>>;
template class NICKSVCHAT_API_EXPORT Parser<std::basic_string<char32_t>>;
#ifdef __cpp_lib_char8_t
template class NICKSVCHAT_API_EXPORT Parser<std::basic_string<char8_t>>;
#endif
 
template class NICKSVCHAT_API_EXPORT Parser<Message>;
 
template class NICKSVCHAT_API_EXPORT Parser<ClientInfo>;
  
template class NICKSVCHAT_API_EXPORT Parser<Request>;
  
template class NICKSVCHAT_API_EXPORT Parser<ClientInfoRequest>;
 
template class NICKSVCHAT_API_EXPORT Parser<MessageRequest>;




}}  /*END OF NAMESPACES*/