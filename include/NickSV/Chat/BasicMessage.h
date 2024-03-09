
#ifndef _NICKSV_CHAT_BASIC_MESSAGE_T
#define _NICKSV_CHAT_BASIC_MESSAGE_T
#pragma once


#include <string>


#include "NickSV/Chat/Types.h"
#include "NickSV/Chat/Utils.h"
#include "NickSV/Chat/Interfaces/ISerializable.h"


namespace NickSV::Chat {


template<typename CharT>
struct BasicMessage : public ISerializable
{
    /*
    Simple data struct.

    */
    static_assert(is_char<CharT>::value, is_char_ERROR_MESSAGE);
    using CharType = CharT;
    using TextType = std::basic_string<CharT>;

    //Simple default constructor
    BasicMessage() = default;
    explicit BasicMessage(const TextType& rsText);
    explicit BasicMessage(TextType&& rsText) noexcept;

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
    Simple equal operators
    */
    CHAT_NODISCARD bool operator==(const BasicMessage<CharT>& other) const;
    CHAT_NODISCARD bool operator!=(const BasicMessage<CharT>& other) const;

    /*
    FIXME add comment
    */
    CHAT_NODISCARD TextType& GetText();
    
    /*
    FIXME add comment
    */
    CHAT_NODISCARD const TextType& GetText() const;

    /*
    FIXME add comment
    */
    const std::unique_ptr<ISerializer> GetSerializer() const override;

private:
	TextType m_sText;
};




} /*END OF NAMESPACES*/




#endif // _NICKSV_CHAT_BASIC_MESSAGE_T

