
#ifndef _NICKSV_CHAT_CLIENT_INFO
#define _NICKSV_CHAT_CLIENT_INFO
#pragma once


#include <string>


#include "NickSV/Chat/Types.h"
#include "NickSV/Chat/Utils.h"
#include "NickSV/Chat/Interfaces/ISerializer.h"


namespace NickSV::Chat {


template<typename CharT>
struct BasicClientInfo : public TriviallySerializable<BasicClientInfo<CharT>>
{
    /*
    Simple data struct.

    App's additional information about a client should be inheritated from this struct.
    One BasicClientInfo per ChatClient class.
    All connceted clients send BasicClientInfo to server.
    Server stores all BasicClientInfo for each connected client.
    */

    static_assert(is_char<CharT>::value, is_char_ERROR_MESSAGE);
    using CharType = CharT;
    using StringType = std::basic_string<CharT>;
    using NickType = std::basic_string<CharT>;
    using APIVersionType = Version_t;
    TRIVIALLY_SERIALIZER_REQUIREMENTS(BasicClientInfo<CharT>)

    //Simple default constructor
    BasicClientInfo() = default;
    explicit BasicClientInfo(const NickType& rsNick);
    explicit BasicClientInfo(NickType&& rsNick) noexcept;

    //Simple copy constructor
    BasicClientInfo(const BasicClientInfo&) = default;
    //Simple move constructor
    BasicClientInfo(BasicClientInfo&&) noexcept;
    //Simple copy assign operator
    const BasicClientInfo<CharT>& operator=(const BasicClientInfo<CharT>&);
    //Simple move assign operator
    BasicClientInfo<CharT>& operator=(BasicClientInfo<CharT>&&) noexcept;
    //Virtual no action destructor for inheritance
    virtual ~BasicClientInfo() = default;

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
    Returns BasicClientInfoSerializer<CharT> that handles "this" BasicClientInfo.
    See BasicClientInfoSerializer<...>
    */

    /*
    Returns CHAT API version
    */
    CHAT_NODISCARD inline APIVersionType GetAPIVer() const;

    /*
    Simple equal operator.
    Returns false if at least one member var is not equal to other one
    and true otherwise
    */
    CHAT_NODISCARD bool operator==(const BasicClientInfo<CharT>& other) const;

    /*
    Simple not equal operator.
    Returns !operator==
    */
    CHAT_NODISCARD bool operator!=(const BasicClientInfo<CharT>& other) const;

    /*
    FIXME add comment
    */
    CHAT_NODISCARD inline NickType& GetNickname();

    /*
    FIXME add comment
    */
    CHAT_NODISCARD inline const NickType& GetNickname() const;

private:
	NickType m_sNick;
    APIVersionType m_nAPIVer = ConvertVersions(NICKSVCHAT_VERSION_MAJOR, NICKSVCHAT_VERSION_MINOR, NICKSVCHAT_VERSION_PATCH, 0);
};






} /*END OF NAMESPACES*/



#endif // _NICKSV_CHAT_CLIENT_INFO