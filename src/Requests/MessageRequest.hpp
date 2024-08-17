
#ifndef _NICKSV_CHAT_MESSAGE_REQUEST_PASTE
#define _NICKSV_CHAT_MESSAGE_REQUEST_PASTE
#pragma once


#include "NickSV/Chat/Serializers/MessageRequestSerializer.h"
#include "NickSV/Chat/Parsers/MessageRequestParser.h"
#include "NickSV/Chat/Requests/MessageRequest.h"

namespace NickSV {
namespace Chat {


//-----------------------------------------------------------------------------------
// Impementation of MessageRequest
//-----------------------------------------------------------------------------------
MessageRequest::MessageRequest() : m_upMessage(new Message()) {};

MessageRequest::MessageRequest(Message msg) : m_upMessage(new Message(std::move(msg))) {};

Message& MessageRequest::GetMessage()
{
    return *m_upMessage;
}

const Message& MessageRequest::GetMessage() const
{
    return *m_upMessage;
}

ERequestType MessageRequest::GetType() const { return ERequestType::Message; }

const  std::unique_ptr<ISerializer> MessageRequest::GetSerializer() const 
{ 
    const auto ptr = new Serializer<MessageRequest>(this);
    return std::unique_ptr<Serializer<MessageRequest>>(ptr);
}

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------




}}  /*END OF NAMESPACES*/




#endif // _NICKSV_CHAT_MESSAGE_REQUEST_PASTE