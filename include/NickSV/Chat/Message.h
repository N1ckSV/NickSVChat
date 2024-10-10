#ifndef _NICKSV_CHAT_MESSAGE_T
#define _NICKSV_CHAT_MESSAGE_T
#pragma once


#include "NickSV/Chat/Types.h"
#include "Message.pb.h"

namespace NickSV {
namespace Chat {


class NICKSVCHAT_API Message
{
public:
    using Proto = ProtoMessage;

    Message() = default;

    const Proto& GetProto() const;
    Proto& GetProto();

    UserID_t SenderID() const;
    void SetSenderID(UserID_t);

    const std::string& Text() const;

    /**
     * Beware to use only UTF-8 character sequences 
     */
    std::string* MutableText();

    /**
     * @retval
     * true if valid UTF-8 string is given, text is set
     * @retval
     * false if invalid UTF-8 string is given, text is not set
     */
    [[nodiscard]] 
    bool SetText(const std::string&);

    /**
     * @retval
     * true if valid UTF-8 string is given, text is set
     * @retval
     * false if invalid UTF-8 string is given, text is not set
     */
    [[nodiscard]] 
    bool SetText(std::string&&);

    bool HasAdditionalData() const;

    template<class T>
    inline bool AdditionalDataIs() const
    { return protoMessage.additional_data().Is<T>(); }

    bool UnpackAdditionalDataTo(google::protobuf::Message*) const; //Pointer bc proto one is ptr;
    void PackAdditionalDataFrom(const google::protobuf::Message&);
private:
    Proto protoMessage;
};











}}  /*END OF NAMESPACES*/




#endif // _NICKSV_CHAT_MESSAGE_T

