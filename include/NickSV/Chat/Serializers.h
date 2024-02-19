

#ifndef _NICKSV_CHAT_SERIALIZERS_T
#define _NICKSV_CHAT_SERIALIZERS_T
#pragma once


#include <cstdint>
#include <string>


#include "NickSV/Chat/Defines.h"
#include "NickSV/Chat/Types.h"


namespace NickSV::Chat {



template<typename CharT>
class Serializer<ClientInfo<CharT>>
{
    /*

    ALIASES: ClientInfoSerializer<>

    Class helper handles the serialization of ClientInfo struct to string 
    and parsing (deserializing) it back.

    Inherit from this class to handle its functions on your custom 
    ClientInfo's child class with your extra info

    */
public:
    using char_type = typename ClientInfo<CharT>::char_type;
    using api_version_type = typename ClientInfo<char_type>::api_version_type;

    Serializer() = delete;
    //Serializer(const Serializer<ClientInfo<CharT>>&) = default;
    //Serializer(Serializer<ClientInfo<CharT>>&&) = default;
    //const Serializer<ClientInfo<CharT>>& operator=(const Serializer<ClientInfo<CharT>>&) = default;
    //const Serializer<ClientInfo<CharT>>& operator=(Serializer<ClientInfo<CharT>>&&) = default;
    explicit Serializer(ClientInfo<CharT>* p_CI);
    virtual ~Serializer(){};

    /*
    Returns the size in bytes of ClientInfo serialization into std::string.
    Used in ToString() and ToStringBuffer(...).
    See OnGetSize(...) to interact:
    Derived::GetSize() == Base::GetSize() + Derived::OnGetSize()
    */
    CHAT_NODISCARD inline size_t GetSize() const;

    /*
    Override it in your derived class. Supposed to return an extra
    info size for serializing.
    Derived::GetSize() == Base::GetSize() + Derived::OnGetSize()
    */
    virtual size_t OnGetSize(size_t baseSize = 0) const;

    /*
    Serializes ClientInfo to std::string which can be converted back deterministically.
    See OnToString(...) to interact
    */
    CHAT_NODISCARD std::string ToString() const;

    /*
    Override it in your derived class and add an extra info.
    pvNewPartStart is supposed to be a pointer to an end of
    serialized ClientInfo string output part.
    Use pvNewPartStart to add an extra info.
    This function is called inside ToString() and ToStringBuffer(...)
    */
    virtual void OnToString(void* pvNewPartStart) const {};

    /*
    Same as ToString() but with externally predefined std::string buffer.
    See OnToString(...) to interact
    */
    void ToStringBuffer(std::string&) const;

    /*
    Parses std::string to ClientInfo (m_pClientInfo).
    See OnParseFromString(...) to interact.
    Return values:
        EResult::kSucces - All good and m_pClientInfo points to
                           a valid ClientInfo or its child value
        EResult::kError  - Something went wrong and m_pClientInfo 
                           points to an unspecified ClientInfo or its child value
                           (some member values could be parsed for debugging)
        OnParseFromString(...) - Returned if all ClientInfo member values
                                 is parsed and valid.
                                 Returns EResult::kSucces by default.
    */
    EResult ParseFromString(const std::string&);

    /*
    Override it in your derived class to parse an extra info.
    pcvNewPartStart is supposed to be a pointer to an end of 
    serialized ClientInfo string input part.
    Use pcvNewPartStart to parse an extra info.
    This function is called inside ParseFromString(...) 
    and MakeFromString(...).
    Returns just EResult::kSucces by default.
    */
    virtual EResult OnParseFromString(const void* pcvNewPartStart);

    /*
    Same as ParseFromString(...) but static and creates 
    new ClientInfo object.
    See OnParseFromString(...) to interact.
    If str is bad, return value is specified as invalid
    */
    static ClientInfo<CharT> MakeFromString(const std::string& str);

private:
    /*
    Pointer to ClientInfo object where "this" serializes from or parses to.
    */
    ClientInfo<CharT>* const m_pClientInfo;
};




} /*END OF NAMESPACES*/



#endif // _NICKSV_CHAT_SERIALIZERS_T