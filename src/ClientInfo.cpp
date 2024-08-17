
#include <string>
#include <cstdint>


#include "NickSV/Chat/Utils.h"
#include "NickSV/Chat/Serializers/ClientInfoSerializer.h"
#include "NickSV/Chat/Parsers/ClientInfoParser.h"
#include "NickSV/Chat/ClientInfo.h"


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

auto ClientInfo::GetUserID()
-> ClientInfo::UserIDType& 
{ return m_nUserID; }

auto ClientInfo::GetUserID() const
-> ClientInfo::UserIDType
{ return m_nUserID; }

auto ClientInfo::GetLibVer()
-> ClientInfo::LibVersionType&
{ return m_nLibVer; }

auto ClientInfo::GetLibVer() const
-> ClientInfo::LibVersionType 
{ return m_nLibVer; }

EState& ClientInfo::GetState()       { return m_eState; }
EState  ClientInfo::GetState() const { return m_eState; }

bool ClientInfo::operator==(const ClientInfo& other) const
{ 
    return m_nLibVer == other.m_nLibVer &&
           m_nUserID == other.m_nUserID &&
           m_eState  == other.m_eState;
}
bool ClientInfo::operator!=(const ClientInfo& other) const { return !operator==(other); }

auto ClientInfo::GetSerializer() const
-> const std::unique_ptr<ISerializer>
{ 
    const auto ptr = new Serializer<ClientInfo>(this);
    return std::unique_ptr<Serializer<ClientInfo>>(ptr);
}


}}  /*END OF NAMESPACES*/