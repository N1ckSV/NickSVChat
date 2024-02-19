

#ifndef _NICKSV_CHAT_CLIENT_INFO
#define _NICKSV_CHAT_CLIENT_INFO
#pragma once


#include <string>
#include <cstdint>


#include "NickSV/Chat/Defines.h"
#include "NickSV/Chat/Types.h"
#include "NickSV/Chat/Utils.h"


namespace NickSV::Chat {


/*
API CLIENT INFO.
*/

template<class CharT>
struct ClientInfo
{
    /*
    Simple data struct.

    App's additional information about a client should be inheritated from this struct.
    One ClientInfo per ChatClient class.
    All connceted clients send ClientInfo to server.
    Server stores all ClientInfo for each connected client.
    */

    static_assert(is_char<CharT>::value, is_char_ERROR_MESSAGE);
    using char_type = CharT;
    using nick_string_type = std::basic_string<CharT>;
    using api_version_type = uint32_t;
    using Serializer_t = ClientInfoSerializer<char_type>;
    friend Serializer_t;

    //Simple default constructor
    ClientInfo() = default;

    //Simple copy constructor
    ClientInfo(const ClientInfo&) = default;
    //Simple move constructor
    ClientInfo(ClientInfo&&) noexcept;
    //Simple copy assign operator
    virtual const ClientInfo<char_type>& operator=(const ClientInfo<char_type>&);
    //Simple move assign operator
    virtual ClientInfo<char_type>& operator=(ClientInfo<char_type>&&) noexcept;
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
    Returns ClientInfoSerializer<char_type> that handles "this" ClientInfo.
    See ClientInfoSerializer<>
    */
    CHAT_NODISCARD virtual Serializer_t GetSerializer();

    /*
    Returns CHAT API version
    */
    CHAT_NODISCARD inline api_version_type GetAPIVer() const;

    /*
    Simple equal operator.
    Returns false if at least one member var is not equal to other one
    and true otherwise
    */
    CHAT_NODISCARD bool operator==(const ClientInfo<char_type>& other) const;

    /*
    Simple not equal operator.
    Returns !operator==
    */
    CHAT_NODISCARD bool operator!=(const ClientInfo<char_type>& other) const;

	nick_string_type m_sNick;
private:
    api_version_type m_nAPIVer = NICKSVCHAT_VERSION;
};






} /*END OF NAMESPACES*/



#endif // _NICKSV_CHAT_CLIENT_INFO