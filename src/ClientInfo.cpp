
#include <string>
#include <cstdint>


#include "NickSV/Chat/Serializers/ClientInfoSerializer.h"
#include "NickSV/Chat/Utils.h"


namespace NickSV {
namespace Chat {


/*
CLIENT INFO.
App's additional information should be inheritated from this struct
*/

ClientInfo::ClientInfo(const UserIDType& id) : m_nUserID(id) {};
ClientInfo::ClientInfo(const UserIDType& id, const EState& state) :
m_eState(state),
m_nUserID(id)
{};

ClientInfo::UserIDType& ClientInfo::GetUserID() { return m_nUserID; }
ClientInfo::UserIDType ClientInfo::GetUserID() const { return m_nUserID; }

ClientInfo::LibVersionType& ClientInfo::GetLibVer() { return m_nLibVer; }
ClientInfo::LibVersionType ClientInfo::GetLibVer() const { return m_nLibVer; }

EState& ClientInfo::GetState() { return m_eState; }
EState ClientInfo::GetState() const { return m_eState; }

bool ClientInfo::operator==(const ClientInfo& other) const
{ 
    return m_nLibVer == other.m_nLibVer &&
           m_nUserID == other.m_nUserID &&
           m_eState  == other.m_eState;
}
bool ClientInfo::operator!=(const ClientInfo& other) const { return !operator==(other); }

const std::unique_ptr<ISerializer> ClientInfo::GetSerializer() const 
{ 
    return std::make_unique<Serializer<ClientInfo>>(this); 
}

}}  /*END OF NAMESPACES*/