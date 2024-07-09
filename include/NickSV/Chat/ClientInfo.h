
#ifndef _NICKSV_CHAT_CLIENT_INFO
#define _NICKSV_CHAT_CLIENT_INFO
#pragma once


#include <string>


#include "NickSV/Chat/Types.h"
#include "NickSV/Chat/Utils.h"
#include "NickSV/Chat/Interfaces/ISerializable.h"


namespace NickSV {
namespace Chat {


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
    using LibVersionType = Version_t;

    ClientInfo() = default;
    explicit ClientInfo(const UserIDType&);
    ClientInfo(const UserIDType&, const EState&);

    DECLARE_RULE_OF_5_VIRTUAL_DEFAULT(ClientInfo);

    /*
    Simple equal operator.
    Returns false if at least one member var is not equal to other one
    and true otherwise
    */
    [[nodiscard]] bool operator==(const ClientInfo& other) const;

    /*
    Simple not equal operator.
    Returns !operator==
    */
    [[nodiscard]] bool operator!=(const ClientInfo& other) const;

    /*
    FIXME add comment
    */
    [[nodiscard]] UserIDType& GetUserID();

    /*
    FIXME add comment
    */
    [[nodiscard]] UserIDType GetUserID() const;
    
    /*
    FIXME add comment
    */
    [[nodiscard]] LibVersionType& GetLibVer();
    
    /*
    FIXME add comment
    */
    [[nodiscard]] LibVersionType GetLibVer() const;

    /*
    FIXME add comment
    */
    [[nodiscard]] EState& GetState();
    
    /*
    FIXME add comment
    */
    [[nodiscard]] EState GetState() const;

    /*
    FIXME add comment
    */
    const std::unique_ptr<ISerializer> GetSerializer() const override;

private:
    LibVersionType m_nLibVer = ConvertVersions(NICKSVCHAT_VERSION_MAJOR, NICKSVCHAT_VERSION_MINOR, NICKSVCHAT_VERSION_PATCH, 0);
    EState         m_eState = EState::Unauthorized;
	UserIDType     m_nUserID;
};


const ClientInfo InvalidClientInfo = ClientInfo(0, EState::Invalid);



}}  /*END OF NAMESPACES*/



#endif // _NICKSV_CHAT_CLIENT_INFO