
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
MessageRequest::MessageRequest() 
    : m_upMessage(Tools::MakeUnique<Message>()) {};

MessageRequest::MessageRequest(Message msg) 
    : m_upMessage(Tools::MakeUnique<Message>(std::move(msg))) {};

Message& MessageRequest::GetMessage()
{ return *m_upMessage; }

auto MessageRequest::GetMessage() const
-> const Message&
{ return *m_upMessage; }

ERequestType MessageRequest::GetType() const
{ return ERequestType::Message; }

auto MessageRequest::GetSerializer() const
-> const std::unique_ptr<ISerializer>
{ 
	return Tools::MakeUnique<Serializer<MessageRequest>>(*this);
}

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------




}}  /*END OF NAMESPACES*/




#endif // _NICKSV_CHAT_MESSAGE_REQUEST_PASTE