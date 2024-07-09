#ifndef _NICKSV_CHAT_CLIENTINFO_REQUEST_SERIALIZER_HEADER
#define _NICKSV_CHAT_CLIENTINFO_REQUEST_SERIALIZER_HEADER
#pragma once


#include <string>

#include "NickSV/Chat/Interfaces/ISerializer.h"
#include "NickSV/Chat/Requests/ClientInfoRequest.h"
#include "NickSV/Chat/Utils.h"


namespace NickSV {
namespace Chat {


template<>
class Serializer<ClientInfoRequest> : public ISerializer
{
    /*


    */
public:
    Serializer() = delete;
    explicit Serializer(const ClientInfoRequest * const);

    inline const ClientInfoRequest* GetObject() const;

    inline size_t GetSize() const override final;
    inline size_t OnGetSize(size_t baseSize = 0) const override;
    
    std::string ToString() const override final;
    std::string::iterator ToString(std::string::iterator,std::string::iterator) const override final;
    inline std::string::iterator OnToString(std::string::iterator, std::string::iterator) const override;
    inline std::string::iterator ToStringBuffer(std::string&) const override final;
private:
    /*
    Pointer to ClientInfoRequest object where "this" serializes from or parses to.
    */
    const ClientInfoRequest* const m_cpcClientInfoRequest; //FIXME maybe const is redundant
};




}}  /*END OF NAMESPACES*/



#endif // _NICKSV_CHAT_CLIENTINFO_REQUEST_SERIALIZER_HEADER