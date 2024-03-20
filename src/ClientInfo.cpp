
#include <string>
#include <cstdint>


#include "NickSV/Chat/Serializers/ClientInfoSerializer.h"
#include "NickSV/Chat/Utils.h"


namespace NickSV::Chat {


/*
API CLIENT INFO.
App's additional information should be inheritated from this struct
*/

ClientInfo::ClientInfo(const UserIDType& id) : m_nUserID(id) {};

ClientInfo::ClientInfo(const UserIDType& id, const EState& state) :
m_eState(state),
m_nUserID(id)
{};

ClientInfo::ClientInfo(ClientInfo&& rhs) noexcept :
m_nAPIVer(rhs.m_nAPIVer),
m_eState(rhs.m_eState),
m_nUserID(rhs.m_nUserID)
{};


const ClientInfo& ClientInfo::operator=(const ClientInfo& rhs)
{
    if (this == std::addressof(rhs)) [[unlikely]] return *this;
    m_nAPIVer = rhs.m_nAPIVer;
    m_nUserID = rhs.m_nUserID;
    m_eState  = rhs.m_eState;
    return *this;
};


ClientInfo& ClientInfo::operator=(ClientInfo&& rhs) noexcept
{
    if (this == std::addressof(rhs)) [[unlikely]] return *this;
    m_nAPIVer = rhs.m_nAPIVer;
    m_nUserID = rhs.m_nUserID;
    m_eState  = rhs.m_eState;
    return *this;
};


ClientInfo::UserIDType& ClientInfo::GetUserID() { return m_nUserID; }


ClientInfo::UserIDType ClientInfo::GetUserID() const { return m_nUserID; }


ClientInfo::APIVersionType& ClientInfo::GetAPIVer() { return m_nAPIVer; }


ClientInfo::APIVersionType ClientInfo::GetAPIVer() const { return m_nAPIVer; }


EState& ClientInfo::GetState() { return m_eState; }


EState ClientInfo::GetState() const { return m_eState; }


bool ClientInfo::operator==(const ClientInfo& other) const
{ 
    return m_nAPIVer == other.m_nAPIVer &&
           m_nUserID == other.m_nUserID &&
           m_eState  == other.m_eState;
}


bool ClientInfo::operator!=(const ClientInfo& other) const { return !operator==(other); }

const std::unique_ptr<ISerializer> ClientInfo::GetSerializer() const 
{ 
    return std::make_unique<Serializer<ClientInfo>>(this); 
}

} /*END OF NAMESPACES*/