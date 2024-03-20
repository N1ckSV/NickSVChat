
#ifndef _NICKSV_CHAT_CLIENT_INFO
#define _NICKSV_CHAT_CLIENT_INFO
#pragma once


#include <string>


#include "NickSV/Chat/Types.h"
#include "NickSV/Chat/Utils.h"
#include "NickSV/Chat/Interfaces/ISerializable.h"


namespace NickSV::Chat {


struct ClientInfo : public ISerializable
{
    /*
    Simple data struct.

    App's additional information about a client should be inheritated from this struct.
    One ClientInfo per ChatClient class.
    All connceted clients send ClientInfo to server.
    Server stores all ClientInfo for each connected client.
    */
    using UserIDType = UserID_t;
    using APIVersionType = Version_t;

    //Simple default constructor
    ClientInfo() = default;
    explicit ClientInfo(const UserIDType&);
    ClientInfo(const UserIDType&, const EState&);

    //Simple copy constructor
    ClientInfo(const ClientInfo&) = default;
    //Simple move constructor
    ClientInfo(ClientInfo&&) noexcept;
    //Simple copy assign operator
    const ClientInfo& operator=(const ClientInfo&);
    //Simple move assign operator
    ClientInfo& operator=(ClientInfo&&) noexcept;
    
    virtual ~ClientInfo() = default;

    /*
    Simple equal operator.
    Returns false if at least one member var is not equal to other one
    and true otherwise
    */
    CHAT_NODISCARD bool operator==(const ClientInfo& other) const;

    /*
    Simple not equal operator.
    Returns !operator==
    */
    CHAT_NODISCARD bool operator!=(const ClientInfo& other) const;

    /*
    FIXME add comment
    */
    CHAT_NODISCARD UserIDType& GetUserID();

    /*
    FIXME add comment
    */
    CHAT_NODISCARD UserIDType GetUserID() const;
    
    /*
    FIXME add comment
    */
    CHAT_NODISCARD APIVersionType& GetAPIVer();
    
    /*
    FIXME add comment
    */
    CHAT_NODISCARD APIVersionType GetAPIVer() const;

    /*
    FIXME add comment
    */
    CHAT_NODISCARD EState& GetState();
    
    /*
    FIXME add comment
    */
    CHAT_NODISCARD EState GetState() const;

    /*
    FIXME add comment
    */
    const std::unique_ptr<ISerializer> GetSerializer() const override;

private:
    APIVersionType m_nAPIVer = ConvertVersions(NICKSVCHAT_VERSION_MAJOR, NICKSVCHAT_VERSION_MINOR, NICKSVCHAT_VERSION_PATCH, 0);
    EState         m_eState = EState::Unauthorized;
	UserIDType     m_nUserID;
};


const ClientInfo InvalidClientInfo = ClientInfo(0, EState::Invalid);



} /*END OF NAMESPACES*/



#endif // _NICKSV_CHAT_CLIENT_INFO