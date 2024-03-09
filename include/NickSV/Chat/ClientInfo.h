
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

    //Simple copy constructor
    ClientInfo(const ClientInfo&) = default;
    //Simple move constructor
    ClientInfo(ClientInfo&&) noexcept;
    //Simple copy assign operator
    const ClientInfo& operator=(const ClientInfo&);
    //Simple move assign operator
    ClientInfo& operator=(ClientInfo&&) noexcept;
    //Virtual no action destructor for inheritance
    virtual ~ClientInfo() = default;

    /*
    Sets object to Invalid state (IsValid() == false).
    m_sNick sets to "Invalid"
    GetAPIVer returs 0
    */
    virtual void SetInvalid();

    /*
    If somehow object was built incorrectly and SetInvalid() is called
    IsValid() tells that.
    REMEMBER - this function does NOT 100% guarantee that the object is VALID,
    but it DOES 100% guarantee if object is INVALID and action should be taken
    */
    CHAT_NODISCARD virtual bool IsValid() const;

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
    const std::unique_ptr<ISerializer> GetSerializer() const override;

private:
	UserID_t m_nUserID;
    Version_t m_nAPIVer = ConvertVersions(NICKSVCHAT_VERSION_MAJOR, NICKSVCHAT_VERSION_MINOR, NICKSVCHAT_VERSION_PATCH, 0);
};






} /*END OF NAMESPACES*/



#endif // _NICKSV_CHAT_CLIENT_INFO