

#include "NickSV/Chat/ClientInfo.h"
#include "NickSV/Chat/Utils.h"


namespace NickSV {
namespace Chat {


ClientInfo::ClientInfo()
{ protoClientInfo.set_lib_version(ConvertVersions(NICKSVCHAT_VERSION_MAJOR, NICKSVCHAT_VERSION_MINOR, NICKSVCHAT_VERSION_PATCH, 0)); }

const ClientInfo& ClientInfo::DefaultInstance()
{
    static const ClientInfo _lib_default_client_info_instance;
    return _lib_default_client_info_instance;
}

EState ClientInfo::State() const
{ return static_cast<EState>(protoClientInfo.state()); }

void ClientInfo::SetState(EState state)
{ protoClientInfo.set_state(static_cast<uint32_t>(state)); }

UserID_t ClientInfo::UserID() const
{ return protoClientInfo.user_id(); }

void ClientInfo::SetUserID(UserID_t id)
{ protoClientInfo.set_user_id(id); }

Version_t ClientInfo::LibVersion() const
{ return protoClientInfo.lib_version(); }


bool ClientInfo::HasAdditionalData() const
{ return protoClientInfo.has_additional_data(); }

bool ClientInfo::UnpackAdditionalDataTo(google::protobuf::Message* pPBMessage)  const
{ return protoClientInfo.additional_data().UnpackTo(pPBMessage); }

void ClientInfo::PackAdditionalDataFrom(const google::protobuf::Message& PBMessage)
{ protoClientInfo.mutable_additional_data()->PackFrom(PBMessage); }

const ClientInfo::Proto& ClientInfo::GetProto() const
{ return protoClientInfo; }

ClientInfo::Proto& ClientInfo::GetProto()
{ return protoClientInfo; }


}}  /*END OF NAMESPACES*/