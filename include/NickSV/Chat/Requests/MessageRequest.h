
#ifndef _NICKSV_CHAT_MESSAGE_REQUEST_T
#define _NICKSV_CHAT_MESSAGE_REQUEST_T
#pragma once

#include <memory>

#include "NickSV/Chat/Message.h"
#include "NickSV/Chat/Requests/Request.h"
#include "NickSV/Chat/Interfaces/ISerializable.h"



namespace NickSV {
namespace Chat {


class NICKSVCHAT_API MessageRequest : public Request
{
    static_assert(is_serializable<Message>::value, is_serializable_ERROR_MESSAGE);
public:
    MessageRequest();
    MessageRequest(Message);

    Message& GetMessage();
    const Message& GetMessage() const;

    ERequestType GetType() const override final;
    const std::unique_ptr<ISerializer> GetSerializer() const override;

    friend Parser<MessageRequest>;
private:
    std::unique_ptr<Message> m_upMessage;
};




}}  /*END OF NAMESPACES*/




#endif // _NICKSV_CHAT_MESSAGE_REQUEST_T