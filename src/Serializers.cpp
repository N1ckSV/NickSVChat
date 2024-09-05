

#include "Serializers/BStringSerializer.hpp"
#include "Serializers/ClientInfoSerializer.hpp"
#include "Serializers/MessageSerializer.hpp"
#include "Serializers/ClientInfoRequestSerializer.hpp"
#include "Serializers/MessageRequestSerializer.hpp"



namespace NickSV {
namespace Chat {


template class NICKSVCHAT_API_EXPORT Serializer<std::basic_string<char>>;
template class NICKSVCHAT_API_EXPORT Serializer<std::basic_string<wchar_t>>;
template class NICKSVCHAT_API_EXPORT Serializer<std::basic_string<char16_t>>;
template class NICKSVCHAT_API_EXPORT Serializer<std::basic_string<char32_t>>;
#ifdef __cpp_lib_char8_t
template class NICKSVCHAT_API_EXPORT Serializer<std::basic_string<char8_t>>;
#endif


template class NICKSVCHAT_API_EXPORT Serializer<ClientInfo>;

template class NICKSVCHAT_API_EXPORT Serializer<Message>;

template class NICKSVCHAT_API_EXPORT Serializer<ClientInfoRequest>;

template class NICKSVCHAT_API_EXPORT Serializer<MessageRequest>;



}}  /*END OF NAMESPACES*/