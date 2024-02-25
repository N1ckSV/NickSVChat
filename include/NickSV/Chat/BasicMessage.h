
#ifndef _NICKSV_CHAT_BASIC_MESSAGE_T
#define _NICKSV_CHAT_BASIC_MESSAGE_T
#pragma once


#include <string>


#include "NickSV/Chat/Types.h"
#include "NickSV/Chat/Utils.h"
#include "NickSV/Chat/Interfaces/ISerializer.h"


namespace NickSV::Chat {


template<typename CharT>
struct BasicMessage : public TriviallySerializable<BasicMessage<CharT>>
{
    /*
    Simple data struct.

    */
    static_assert(is_char<CharT>::value, is_char_ERROR_MESSAGE);
    using CharType = CharT;
    using text_type = std::basic_string<CharT>;
    TRIVIALLY_SERIALIZER_REQUIREMENTS(BasicMessage<CharT>);

    //Simple default constructor
    BasicMessage() = default;
    explicit BasicMessage(const text_type& rsText);
    explicit BasicMessage(text_type&& rsText) noexcept;

    //Simple copy constructor
    BasicMessage(const BasicMessage&) = default;
    //Simple move constructor
    BasicMessage(BasicMessage&&) noexcept;
    //Simple copy assign operator
    const BasicMessage<CharT>& operator=(const BasicMessage<CharT>&);
    //Simple move assign operator
    BasicMessage<CharT>& operator=(BasicMessage<CharT>&&) noexcept;
    //Virtual no action destructor for inheritance
    virtual ~BasicMessage() = default;

    /*
    Sets object to Invalid state (IsValid() == false).
    m_sNick sets to "Invalid"
    GetAPIVer returs 0
    */
    //virtual void SetInvalid();

    /*
    If somehow object was built incorrectly and SetInvalid() is called
    IsValid() tells that.
    REMEMBER - this function does NOT 100% guarantee that the object is VALID,
    but it DOES 100% guarantee if object is INVALID and action should be taken
    */
    //CHAT_NODISCARD virtual bool IsValid() const;

    /*
    Returns ClientInfoSerializer<CharType> that handles "this" ClientInfo.
    See ClientInfoSerializer<>
    */

    /*
    Returns CHAT API version
    */
    //CHAT_NODISCARD inline APIVersionType GetAPIVer() const;

    /*
    Simple equal operators
    */
    CHAT_NODISCARD bool operator==(const BasicMessage<CharT>& other) const;
    CHAT_NODISCARD bool operator!=(const BasicMessage<CharT>& other) const;

    /*
    FIXME add comment
    */
    CHAT_NODISCARD inline text_type& GetText();
    
    /*
    FIXME add comment
    */
    CHAT_NODISCARD inline const text_type& GetText() const;


private:
	text_type m_sText;
};




} /*END OF NAMESPACES*/




#endif // _NICKSV_CHAT_BASIC_MESSAGE_T

