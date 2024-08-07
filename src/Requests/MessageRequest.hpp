
#ifndef _NICKSV_CHAT_MESSAGE_REQUEST_PASTE
#define _NICKSV_CHAT_MESSAGE_REQUEST_PASTE
#pragma once


#include "NickSV/Chat/Requests/MessageRequest.h"
#include "NickSV/Chat/Serializers/MessageRequestSerializer.h"

namespace NickSV {
namespace Chat {


//-----------------------------------------------------------------------------------
// Impementation of MessageRequest
//-----------------------------------------------------------------------------------
MessageRequest::MessageRequest() : m_upMessage(std::make_unique<Message>()) {};

MessageRequest::MessageRequest(Message msg) : m_upMessage(std::make_unique<Message>(std::move(msg))) {};

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
    return std::make_unique<Serializer<MessageRequest>>(this); 
}
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------




}}  /*END OF NAMESPACES*/




#endif // _NICKSV_CHAT_MESSAGE_REQUEST_PASTE