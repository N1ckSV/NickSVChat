
#ifndef _NICKSV_CHAT_MESSAGE_REQUEST_T
#define _NICKSV_CHAT_MESSAGE_REQUEST_T
#pragma once

#include <memory>

#include "NickSV/Chat/BasicMessage.h"
#include "NickSV/Chat/Requests/Request.h"
#include "NickSV/Chat/Interfaces/ISerializable.h"



namespace NickSV::Chat {


class MessageRequest : public Request
{
    static_assert(is_serializable<Message>::value, is_serializable_ERROR_MESSAGE);
public:
    MessageRequest();

    std::unique_ptr<Message>& GetMessage();
    const std::unique_ptr<Message>& GetMessage() const;

    ERequestType GetType() const override;
    const std::unique_ptr<ISerializer> GetSerializer() const override;
private:
    std::unique_ptr<Message> m_upMessage;
};




} /*END OF NAMESPACES*/




#endif // _NICKSV_CHAT_MESSAGE_REQUEST_T