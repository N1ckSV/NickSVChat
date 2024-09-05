#include <string>
#include <cstdint>


#include "NickSV/Chat/Utils.h"
#include "NickSV/Chat/Serializers/MessageSerializer.h"
#include "NickSV/Chat/Parsers/MessageParser.h"
#include "NickSV/Chat/Message.h"


namespace NickSV {
namespace Chat {


/*
MESSAGE.
Struct that user is sending to server
*/

Message::Message(Message::UserIDType senderID) : m_nSenderID(senderID) {};

Message::Message(Message::TextType rsText) : m_sText(std::move(rsText)) {};

Message::Message(Message::UserIDType senderID, Message::TextType rsText) 
    :  m_nSenderID(senderID), m_sText(std::move(rsText)) {};

bool inline Message::operator==(const Message& other) const
{ 
    constexpr size_t size = sizeof(m_sText) + sizeof(m_nSenderID) + sizeof(void*);
    Tools::type_integrity_assert_virtual<Message, COMPILER_AWARE_VALUE(size,size,size)>();
    return (m_sText == other.m_sText) &&
           (m_nSenderID == other.m_nSenderID);
}

bool Message::operator!=(const Message& other) const
{ return !operator==(other); }

auto Message::GetText() const
-> const typename Message::TextType&
{ return m_sText; }

auto Message::GetText() 
-> typename Message::TextType&
{ return m_sText; }

auto Message::GetSenderID()
-> typename Message::UserIDType&
{ return m_nSenderID; }

auto Message::GetSenderID() const
-> typename Message::UserIDType
{ return m_nSenderID; }

auto Message::GetSerializer() const
-> const std::unique_ptr<ISerializer>
{
    return Tools::MakeUnique<Serializer<Message>>(*this);
}


}}  /*END OF NAMESPACES*/