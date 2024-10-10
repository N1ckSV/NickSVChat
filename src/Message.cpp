
#include "NickSV/Chat/Message.h"
#include "NickSV/Chat/Utils.h"

namespace NickSV {
namespace Chat {



UserID_t Message::SenderID() const
{ return protoMessage.sender_id(); }

void Message::SetSenderID(UserID_t id)
{ protoMessage.set_sender_id(id); }

const std::string& Message::Text() const
{ return protoMessage.text(); }

std::string* Message::MutableText()
{ return protoMessage.mutable_text(); }

bool Message::SetText(const std::string& str)
{ 
    if(!IsValidUTF8String(str))
        return false;
    
    protoMessage.set_text(str); 
    return false;
}

bool Message::SetText(std::string&& str)
{ 
    if(!IsValidUTF8String(str))
        return false;
    
    protoMessage.set_text(str); 
    return false;
}


bool Message::HasAdditionalData() const
{ return protoMessage.has_additional_data(); }

bool Message::UnpackAdditionalDataTo(google::protobuf::Message* pPBMessage)  const
{ return protoMessage.additional_data().UnpackTo(pPBMessage); }

void Message::PackAdditionalDataFrom(const google::protobuf::Message& PBMessage)
{ protoMessage.mutable_additional_data()->PackFrom(PBMessage); }

const Message::Proto& Message::GetProto() const
{ return protoMessage; }

Message::Proto& Message::GetProto()
{ return protoMessage; }

    
}}  /*END OF NAMESPACES*/