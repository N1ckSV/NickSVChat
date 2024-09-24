


#include "NickSV/Chat/Request.h"
#include "NickSV/Chat/ClientInfo.h"
#include "NickSV/Chat/Message.h"



namespace NickSV {
namespace Chat {


Request::Request(ERequestType type)
{ protoRequest.set_type(static_cast<uint32>(type)); }

ERequestType Request::Type() const
{ return static_cast<ERequestType>(protoRequest.type()); }

std::string Request::SerializeAsString() const
{
    std::string strReques;
    return protoRequest.SerializeToString(&strReques) ? strReques : "";
}

bool Request::SerializeToString(std::string* pString) const
{
    auto size = protoRequest.ByteSizeLong();
    pString->resize(size);
	bool serializeResult = protoRequest.SerializeToArray(pString->data(), static_cast<int>(size));
	CHAT_EXPECT(serializeResult, "Failed to serialize Request");
    return serializeResult;
}

bool Request::ParseFromString(const std::string& str)
{ return protoRequest.ParseFromArray(str.data(), static_cast<int>(str.size())); }

bool Request::HasData() const
{ return protoRequest.has_data(); }

bool Request::UnpackDataTo(google::protobuf::Message* pPBMessage) const
{ return protoRequest.data().UnpackTo(pPBMessage); }

void Request::PackDataFrom(const google::protobuf::Message& PBMessage)
{ protoRequest.mutable_data()->PackFrom(PBMessage); }

const Request::Proto& Request::GetProto() const
{ return protoRequest; }

Request::Proto& Request::GetProto()
{ return protoRequest; }




ClientInfoRequest::ClientInfoRequest() : Request(ERequestType::ClientInfo) {};

bool ClientInfoRequest::HasClientInfo() const
{
    CHAT_ASSERT(HasData() ? DataIs<NickSV::Chat::ClientInfo::Proto>() : true, 
        "ClientInfoRequest has data, but this is not ClientInfo");
    return HasData();
}

void ClientInfoRequest::PackClientInfoFrom(const ClientInfo& clientInfo)
{ PackDataFrom(clientInfo.GetProto()); }

bool ClientInfoRequest::UnpackClientInfoTo(ClientInfo* pClientInfo) const
{ return UnpackDataTo(&(pClientInfo->GetProto())); }






MessageRequest::MessageRequest() : Request(ERequestType::Message) {};

bool MessageRequest::HasMessage() const
{
    CHAT_ASSERT(HasData() ? DataIs<NickSV::Chat::Message::Proto>() : true, 
        "MessageRequest has data, but this is not Message");
    return HasData();
}

void MessageRequest::PackMessageFrom(const NickSV::Chat::Message& message)
{ PackDataFrom(message.GetProto()); }

bool MessageRequest::UnpackMessageTo(NickSV::Chat::Message* pMessage) const
{ return UnpackDataTo(&pMessage->GetProto()); }






}}  /*END OF NAMESPACES*/