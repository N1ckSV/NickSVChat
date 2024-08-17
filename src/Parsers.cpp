
#include <cstring>
#include <memory>

#include "NickSV/Chat/Utils.h"

#include "Parsers/BStringParser.hpp"
#include "Parsers/ClientInfoParser.hpp"
#include "Parsers/BMessageParser.hpp"
#include "Parsers/RequestParser.hpp"
#include "Parsers/ClientInfoRequestParser.hpp"
#include "Parsers/MessageRequestParser.hpp"



namespace NickSV {
namespace Chat {


template class NICKSVCHAT_API Parser<std::basic_string<char>>;
template class NICKSVCHAT_API Parser<std::basic_string<wchar_t>>;
template class NICKSVCHAT_API Parser<std::basic_string<char16_t>>;
template class NICKSVCHAT_API Parser<std::basic_string<char32_t>>;
 
template class NICKSVCHAT_API Parser<BasicMessage<char>>;
template class NICKSVCHAT_API Parser<BasicMessage<wchar_t>>;
template class NICKSVCHAT_API Parser<BasicMessage<char16_t>>;
template class NICKSVCHAT_API Parser<BasicMessage<char32_t>>;
 
template class NICKSVCHAT_API Parser<ClientInfo>;
  
template class NICKSVCHAT_API Parser<Request>;
  
template class NICKSVCHAT_API Parser<ClientInfoRequest>;
 
template class NICKSVCHAT_API Parser<MessageRequest>;





template ClientInfo                  MakeFromString<ClientInfo>(const std::string&);
     
template BasicMessage<char>          MakeFromString<BasicMessage<char>>(const std::string&);
template BasicMessage<wchar_t>       MakeFromString<BasicMessage<wchar_t>>(const std::string&);
template BasicMessage<char16_t>      MakeFromString<BasicMessage<char16_t>>(const std::string&);
template BasicMessage<char32_t>      MakeFromString<BasicMessage<char32_t>>(const std::string&);

template std::basic_string<char>     MakeFromString<std::basic_string<char>>(const std::string&);
template std::basic_string<wchar_t>  MakeFromString<std::basic_string<wchar_t>>(const std::string&);
template std::basic_string<char16_t> MakeFromString<std::basic_string<char16_t>>(const std::string&);
template std::basic_string<char32_t> MakeFromString<std::basic_string<char32_t>>(const std::string&);

}}  /*END OF NAMESPACES*/