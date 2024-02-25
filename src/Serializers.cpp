
#include <cstring>
#include <memory>



#include "Serializers/BStringSerializer.hpp"
#include "Serializers/BClientInfoSerializer.hpp"
#include "Serializers/BMessageSerializer.hpp"



namespace NickSV::Chat {




template<typename ToSerialize>
ToSerialize MakeFromString(const std::string& str)
{
    static_assert(is_serializable<ToSerialize>::value, is_serializable_ERROR_MESSAGE);
    ToSerialize toSerialize;
    if(toSerialize.GetSerializer()->ParseFromString(str) > str.begin())
        return toSerialize;
    return ToSerialize();
}




//----------------------------------------------------------------------------------------------------
// TriviallySerializable<ToSerialize> implementation
//----------------------------------------------------------------------------------------------------
template<typename ToSerialize>
std::unique_ptr<ISerializer> TriviallySerializable<ToSerialize>::GetSerializer()
{
    return std::make_unique<SerializerType>(static_cast<ToSerialize*>(this));
}

template<typename ToSerialize>
const std::unique_ptr<ISerializer> TriviallySerializable<ToSerialize>::GetSerializer() const
{
    return std::make_unique<ConstSerializerType>(static_cast<const ToSerialize*>(this));
}

template<typename ToSerialize>
const std::unique_ptr<ISerializer> TriviallySerializable<ToSerialize>::GetConstSerializer() const
{
    return std::make_unique<ConstSerializerType>(static_cast<const ToSerialize*>(this));
}
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------



template BasicClientInfo<char>      MakeFromString<BasicClientInfo<char>>(const std::string&);
template BasicClientInfo<wchar_t>   MakeFromString<BasicClientInfo<wchar_t>>(const std::string&);
template BasicClientInfo<char16_t>  MakeFromString<BasicClientInfo<char16_t>>(const std::string&);
template BasicClientInfo<char32_t>  MakeFromString<BasicClientInfo<char32_t>>(const std::string&);

template BasicMessage<char>         MakeFromString<BasicMessage<char>>(const std::string&);
template BasicMessage<wchar_t>      MakeFromString<BasicMessage<wchar_t>>(const std::string&);
template BasicMessage<char16_t>     MakeFromString<BasicMessage<char16_t>>(const std::string&);
template BasicMessage<char32_t>     MakeFromString<BasicMessage<char32_t>>(const std::string&);


} /*END OF NAMESPACES*/