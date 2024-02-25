
#ifndef _NICKSV_CHAT_ISERIALIZER_T
#define _NICKSV_CHAT_ISERIALIZER_T
#pragma once


#include <memory>


#include "NickSV/Chat/Types.h"




namespace NickSV::Chat {




//------------------------------------------------------------------------------------
// Serializer interface
//------------------------------------------------------------------------------------
class ISerializer
{
public:
    CHAT_NODISCARD virtual inline               size_t GetSize() const = 0;
    CHAT_NODISCARD virtual inline               size_t OnGetSize(size_t baseSize = 0) const = 0;

    CHAT_NODISCARD virtual                 std::string ToString() const = 0;
    CHAT_NODISCARD virtual       std::string::iterator ToString(std::string::iterator, std::string::iterator) const = 0;
    CHAT_NODISCARD virtual       std::string::iterator OnToString(std::string::iterator, std::string::iterator) const = 0;
    CHAT_NODISCARD virtual       std::string::iterator ToStringBuffer(std::string&) const = 0;

    CHAT_NODISCARD virtual std::string::const_iterator ParseFromString(const std::string&) = 0;
    CHAT_NODISCARD virtual std::string::const_iterator ParseFromString(std::string::const_iterator, std::string::const_iterator) = 0;
    CHAT_NODISCARD virtual std::string::const_iterator OnParseFromString(std::string::const_iterator, std::string::const_iterator) = 0;
                   virtual                             ~ISerializer() = default;
};
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------




//------------------------------------------------------------------------------------
// Interface for structs, if you need to make them serializable
//------------------------------------------------------------------------------------
class ISerializable
{
public:
    CHAT_NODISCARD virtual       std::unique_ptr<ISerializer> GetSerializer() = 0;
    CHAT_NODISCARD virtual const std::unique_ptr<ISerializer> GetSerializer() const = 0;
    CHAT_NODISCARD virtual const std::unique_ptr<ISerializer> GetConstSerializer() const = 0;
};
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------








//------------------------------------------------------------------------------------
// TriviallySerializable is Trivially ISerializable implementation
//------------------------------------------------------------------------------------

// EXPAMLES OF USING TriviallySerializable: BasicClientInfo<...>, BasicMessage<...>

// TriviallySerializable requirements.
// Paste this into your data struct [with TYPE = class name] for TriviallySerializable to work
#define TRIVIALLY_SERIALIZER_REQUIREMENTS(TYPE)                                                    \
    using SerializerType = typename TriviallySerializable<TYPE>::SerializerType;                   \
    using ConstSerializerType = typename TriviallySerializable<TYPE>::ConstSerializerType;         \
    friend typename TriviallySerializable<TYPE>::SerializerType;                                   \
    friend typename TriviallySerializable<TYPE>::ConstSerializerType;                              \

template<typename ToSerialize>
class TriviallySerializable : public ISerializable
{
public:
    using SerializerType = Serializer<ToSerialize>;
    using ConstSerializerType = ConstSerializer<ToSerialize>;

    //Returns unique_ptr to Serializer<ToSerialize>
    std::unique_ptr<ISerializer> GetSerializer() override;
    //Returns unique_ptr to ConstSerializer<ToSerialize>
    const std::unique_ptr<ISerializer> GetSerializer() const override;
    //Returns unique_ptr to ConstSerializer<ToSerialize>
    const std::unique_ptr<ISerializer> GetConstSerializer() const override;
};
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------





} /*END OF NAMESPACES*/




#endif // _NICKSV_CHAT_ISERIALIZER_T