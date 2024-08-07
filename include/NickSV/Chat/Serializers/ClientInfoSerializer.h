#ifndef _NICKSV_CHAT_CLIENTINFO_SERIALIZER_HEADER
#define _NICKSV_CHAT_CLIENTINFO_SERIALIZER_HEADER
#pragma once


#include <string>

#include "NickSV/Chat/Interfaces/ISerializer.h"
#include "NickSV/Chat/ClientInfo.h"
#include "NickSV/Chat/Utils.h"


namespace NickSV {
namespace Chat {



template<>
class Serializer<ClientInfo> : public ISerializer
{
    /*

    ALIASES: ClientInfoSerializer<>

    Class helper handles the serialization of ClientInfo struct to string 
    and parsing (deserializing) it back.

    Inherit from this class to handle its functions on your custom 
    ClientInfo's child class with your extra info

    */
public:
    using UserIDType = typename ClientInfo::UserIDType;
    using LibVersionType = typename ClientInfo::LibVersionType;

    Serializer() = delete;
    explicit Serializer(ClientInfo const * const);

    inline const ClientInfo * GetObject() const;

    /*
    Returns the size in bytes of ClientInfo serialization into std::string.
    Used in ToString() and ToStringBuffer(...).
    See OnGetSize(...) to interact:
    Derived::GetSize() == Base::GetSize() + Derived::OnGetSize()
    */
    inline size_t GetSize() const override final;

    /*
    Override it in your derived class. Supposed to return an extra
    info size for serializing.
    Derived::GetSize() == Base::GetSize() + Derived::OnGetSize()
    */
    inline size_t OnGetSize(size_t baseSize = 0) const override;
    
    /*
    Serializes ClientInfo to std::string which can be converted back deterministically.
    See OnToString(...) to interact
    */
    std::string ToString() const override final;

    
    std::string::iterator ToString(std::string::iterator,std::string::iterator) const override final;

    /*
    Override it in your derived class and add an extra info.
    pvNewPartStart is supposed to be a pointer to an end of
    serialized ClientInfo string output part.
    Use pvNewPartStart to add an extra info.
    This function is called inside ToString() and ToStringBuffer(...)
    */
    inline std::string::iterator OnToString(std::string::iterator, std::string::iterator) const override;

    /*
    Same as ToString() but with externally predefined std::string buffer.
    See OnToString(...) to interact
    */
    inline std::string::iterator ToStringBuffer(std::string&) const override final;
private:
    /*
    Pointer to ClientInfo object where "this" serializes from or parses to.
    */
    const ClientInfo* const m_cpcClientInfo;
};




}}  /*END OF NAMESPACES*/



#endif // _NICKSV_CHAT_CLIENTINFO_SERIALIZER_HEADER