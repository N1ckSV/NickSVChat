
#ifndef _NICKSV_CHAT_BASIC_MESSAGE_T
#define _NICKSV_CHAT_BASIC_MESSAGE_T
#pragma once


#include <string>


#include "NickSV/Chat/Types.h"
#include "NickSV/Chat/Utils.h"
#include "NickSV/Chat/Interfaces/ISerializable.h"

#include "NickSV/Tools/TypeTraits.h"


namespace NickSV {
namespace Chat {


template<typename CharT>
struct BasicMessage : public ISerializable
{
    /*
    Simple data struct.

    */
    static_assert(Tools::is_char<CharT>::value, is_char_ERROR_MESSAGE);
    using CharType = CharT;
    using TextType = std::basic_string<CharT>;
    using UserIDType = UserID_t;

    //Simple default constructor
    BasicMessage() = default;
    explicit BasicMessage(const TextType& rsText);
    explicit BasicMessage(TextType&& rsText) noexcept;

    
    DECLARE_RULE_OF_5_VIRTUAL_DEFAULT(BasicMessage);
    
    /*
    Simple equal operators
    */
    [[nodiscard]] bool operator==(const BasicMessage<CharT>& other) const;
    [[nodiscard]] bool operator!=(const BasicMessage<CharT>& other) const;

    /*
    FIXME add comment
    */
    [[nodiscard]] TextType& GetText();
    
    /*
    FIXME add comment
    */
    [[nodiscard]] const TextType& GetText() const;

    /*
    FIXME add comment
    */
    const std::unique_ptr<ISerializer> GetSerializer() const override;

private:
    //UserIDType m_nUserID;
	TextType m_sText;
};




}}  /*END OF NAMESPACES*/




#endif // _NICKSV_CHAT_BASIC_MESSAGE_T

