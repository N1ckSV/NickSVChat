
#ifndef _NICKSV_CHAT_MESSAGE_T
#define _NICKSV_CHAT_MESSAGE_T
#pragma once


#include <string>


#include "NickSV/Chat/Types.h"
#include "NickSV/Chat/Utils.h"
#include "NickSV/Chat/Interfaces/ISerializable.h"

#include "NickSV/Tools/TypeTraits.h"


namespace NickSV {
namespace Chat {


struct NICKSVCHAT_API Message : public ISerializable
{
    /*
    Simple data struct.

    */
    static_assert(Tools::is_char<CHAT_CHAR>::value, is_char_ERROR_MESSAGE);
    using CharType = CHAT_CHAR;
    using TextType = std::basic_string<CHAT_CHAR>;
    using UserIDType = UserID_t;

    //Simple default constructor
    Message() = default;
    explicit Message(UserIDType senderID);
    explicit Message(TextType rsText);
    Message(UserIDType senderID, TextType rsText);

    
    DECLARE_RULE_OF_5_VIRTUAL_DEFAULT(Message);
    
    /*
    Simple equal operators
    */
    virtual bool operator==(const Message& other) const;
    bool operator!=(const Message& other) const;

    /*
    FIXME add comment
    */
    TextType& GetText();
    
    /*
    FIXME add comment
    */
    const TextType& GetText() const;

    
    /*
    FIXME add comment
    */
    UserIDType& GetSenderID();

    /*
    FIXME add comment
    */
    UserIDType GetSenderID() const;

    /*
    FIXME add comment
    */
    const std::unique_ptr<ISerializer> GetSerializer() const override;

private:
    UserIDType m_nSenderID = 0;
	TextType m_sText;
};




}}  /*END OF NAMESPACES*/




#endif // _NICKSV_CHAT_MESSAGE_T

