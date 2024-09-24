#ifndef _NICKSV_CHAT_CLIENT_INFO
#define _NICKSV_CHAT_CLIENT_INFO
#pragma once


#include "NickSV/Chat/Types.h"
#include "ClientInfo.pb.h"



namespace NickSV {
namespace Chat {


class NICKSVCHAT_API ClientInfo
{
public:
    using Proto = ProtoClientInfo;
    
    ClientInfo();

    const Proto& GetProto() const;
    Proto& GetProto();

    static const ClientInfo& DefaultInstance();

    EState State() const;
    void SetState(EState);

    UserID_t UserID() const;
    void SetUserID(UserID_t);

    Version_t LibVersion() const;

    bool HasAdditionalData() const;

    template<class T>
    bool AdditionalDataIs() const
    { return protoClientInfo.additional_data().Is<T>(); }

    bool UnpackAdditionalDataTo(google::protobuf::Message*) const; //Pointer bc proto one is ptr;
    void PackAdditionalDataFrom(const google::protobuf::Message&);

private:
    Proto protoClientInfo;
};


}}  /*END OF NAMESPACES*/



#endif // _NICKSV_CHAT_CLIENT_INFO