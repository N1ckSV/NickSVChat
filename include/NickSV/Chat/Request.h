
#ifndef _NICKSV_CHAT_REQUEST
#define _NICKSV_CHAT_REQUEST
#pragma once


#include "NickSV/Chat/Types.h"

#include "NickSV/Chat/Proto/Request.pb.h"


namespace NickSV {
namespace Chat {

class NICKSVCHAT_API Request
{
public:
    using Proto = ProtoRequest;

    explicit Request(ERequestType type = ERequestType::Unknown);

    ERequestType Type() const;
    std::string SerializeAsString() const;
    bool SerializeToString(std::string*) const;
    bool ParseFromString(const std::string&);
    
    const Proto& GetProto() const;
    Proto& GetProto();
protected:

    bool HasData() const;

    template<class T>
    inline bool DataIs() const
    { return protoRequest.data().Is<T>(); }

    bool UnpackDataTo(google::protobuf::Message*) const; //Pointer bc proto one is ptr;
    void PackDataFrom(const google::protobuf::Message&);
private:
    Proto protoRequest;
};




class NICKSVCHAT_API ClientInfoRequest : public Request
{
public:
    ClientInfoRequest();

    bool HasClientInfo() const;

    void PackClientInfoFrom(const ClientInfo& clientInfo);

    bool UnpackClientInfoTo(ClientInfo* clientInfo) const;
};



class NICKSVCHAT_API MessageRequest : public Request
{
public:
    MessageRequest();

    bool HasMessage() const;

    void PackMessageFrom(const NickSV::Chat::Message&);

    bool UnpackMessageTo(NickSV::Chat::Message*) const;
};




}}  /*END OF NAMESPACES*/


#endif //_NICKSV_CHAT_REQUEST