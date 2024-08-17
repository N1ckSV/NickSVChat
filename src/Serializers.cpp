

#include "Serializers/BStringSerializer.hpp"
#include "Serializers/ClientInfoSerializer.hpp"
#include "Serializers/BMessageSerializer.hpp"
#include "Serializers/MessageRequestSerializer.hpp"
#include "Serializers/ClientInfoRequestSerializer.hpp"



namespace NickSV {
namespace Chat {


template class NICKSVCHAT_API Serializer<std::basic_string<char>>;
template class NICKSVCHAT_API Serializer<std::basic_string<wchar_t>>;
template class NICKSVCHAT_API Serializer<std::basic_string<char16_t>>;
template class NICKSVCHAT_API Serializer<std::basic_string<char32_t>>;

template class NICKSVCHAT_API Serializer<BasicMessage<char>>;
template class NICKSVCHAT_API Serializer<BasicMessage<wchar_t>>;
template class NICKSVCHAT_API Serializer<BasicMessage<char16_t>>;
template class NICKSVCHAT_API Serializer<BasicMessage<char32_t>>;

template class NICKSVCHAT_API Serializer<ClientInfo>;

template class NICKSVCHAT_API Serializer<ClientInfoRequest>;

template class NICKSVCHAT_API Serializer<MessageRequest>;



}}  /*END OF NAMESPACES*/