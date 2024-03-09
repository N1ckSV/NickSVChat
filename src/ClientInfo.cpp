
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


ClientInfo::ClientInfo(ClientInfo&& rhs) noexcept :  m_nUserID(rhs.m_nUserID), m_nAPIVer(rhs.m_nAPIVer) {};


inline void ClientInfo::SetInvalid()
{
    m_nUserID = 0;
}


const ClientInfo& ClientInfo::operator=(const ClientInfo& rhs)
{
    if (this == std::addressof(rhs)) [[unlikely]] return *this;
    m_nAPIVer = rhs.m_nAPIVer;
    m_nUserID = rhs.m_nUserID;
    return *this;
};


ClientInfo& ClientInfo::operator=(ClientInfo&& rhs) noexcept
{
    if (this == std::addressof(rhs)) [[unlikely]] return *this;
    m_nAPIVer = rhs.m_nAPIVer;
    m_nUserID = rhs.m_nUserID;
    return *this;
};


ClientInfo::UserIDType& ClientInfo::GetUserID() { return m_nUserID; }


ClientInfo::UserIDType ClientInfo::GetUserID() const { return m_nUserID; }


bool ClientInfo::IsValid() const { return GetUserID() > 0; }


ClientInfo::APIVersionType& ClientInfo::GetAPIVer() { return m_nAPIVer; }


ClientInfo::APIVersionType ClientInfo::GetAPIVer() const { return m_nAPIVer; }


bool ClientInfo::operator==(const ClientInfo& other) const
{ 
    return m_nAPIVer == other.m_nAPIVer &&
           m_nUserID == other.m_nUserID;
}


bool ClientInfo::operator!=(const ClientInfo& other) const { return !operator==(other); }

const std::unique_ptr<ISerializer> ClientInfo::GetSerializer() const 
{ 
    return std::make_unique<Serializer<ClientInfo>>(this); 
}

} /*END OF NAMESPACES*/