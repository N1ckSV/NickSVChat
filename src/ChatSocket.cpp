
#include <iostream>

#include <steam/steamnetworkingsockets.h>

#include "NickSV/Chat/ChatSocket.h"

#include "NickSV/Chat/Requests/Request.h"
#include "NickSV/Chat/ClientInfo.h"

#include "NickSV/Chat/Serializers/ClientInfoRequestSerializer.h"
#include "NickSV/Chat/Serializers/MessageRequestSerializer.h"


namespace NickSV::Chat {

EResult ChatSocket::Run(const ChatIPAddr &serverAddr)
{
	CHAT_ASSERT(!IsRunning(), "Connection is already running");
	CHAT_ASSERT(!m_pConnectionThread, "m_pConnectionThread is not nullptr");
	CHAT_ASSERT(!m_pRequestThread, "m_pRequestThread is not nullptr");
    return EResult::Success;
}

void ChatSocket::CloseConnection()
{
    CHAT_EXPECT(m_pConnectionThread, "Connection thread is missing (m_pConnectionThread == null).");
    CHAT_EXPECT(m_pRequestThread, "Request thread is missing (m_pRequestThread == null).");

    this->OnPreCloseConnection();
    ///////////////////////////////////
    //TODO
    //FIXME FIXME FIXME FIXME FIXME FIXME
    m_bGoingExit = true;
	if(m_pConnectionThread)
   		m_pConnectionThread->join();
	
	if(m_pRequestThread)
   		m_pRequestThread->join();

    delete m_pConnectionThread;
    delete m_pRequestThread;
    m_pConnectionThread = nullptr;
    m_pRequestThread = nullptr;
    ///////////////////////////////////
    this->OnCloseConnection();
}

bool inline ChatSocket::IsRunning() { return !m_bGoingExit; }

ChatSocket::ChatSocket()
{
    m_bGoingExit = true;
    m_pConnectionThread = nullptr;
    m_pRequestThread = nullptr;
    
	/////////
}

ChatSocket::~ChatSocket()
{
    delete m_pConnectionThread;
	delete m_pRequestThread;
}

EResult ChatSocket::OnPreRun(const ChatIPAddr &serverAddr, ChatErrorMsg &errMsg)  					 { return EResult::Success; };
EResult ChatSocket::OnRun(const ChatIPAddr &serverAddr, EResult outsideResult, ChatErrorMsg &errMsg) { return outsideResult; };
void ChatSocket::OnPreCloseConnection()  { };
void ChatSocket::OnCloseConnection()     { };
EResult ChatSocket::OnPreHandleRequest(const Request*, RequestInfo&)					{ return EResult::Success; }
EResult ChatSocket::OnHandleRequest(const Request*, RequestInfo, EResult outsideResult)	{ return outsideResult; }
EResult ChatSocket::OnPreQueueRequest(const Request*, RequestInfo&)						{ return EResult::Success; }
EResult ChatSocket::OnQueueRequest(const Request*, RequestInfo, EResult outsideResult)	{ return outsideResult; }

EResult ChatSocket::QueueRequest(const Request* pcReq, RequestInfo rInfo)
{
	auto result = OnPreQueueRequest(pcReq, rInfo);
	if(result != EResult::Success)
		return OnQueueRequest(pcReq, rInfo, result);

	CHAT_ASSERT(IsRunning(), "Function supposed to work while connection is running");
	CHAT_EXPECT(pcReq, "Function called with (Request*) pcReq == nullptr");
	CHAT_EXPECT(pcReq->GetType() != ERequestType::Unknown, 
		"Function supposed to work with known request");

	if(!pcReq || pcReq->GetType() == ERequestType::Unknown)
		return OnQueueRequest(pcReq, rInfo, EResult::InvalidParam);
	
	CHAT_EXPECT(m_sendRequestsQueue.size() < Constant::MaxSendRequestsQueueSize, 
		"Function called but queue is overflowed");
	if(m_sendRequestsQueue.size() >= Constant::MaxSendRequestsQueueSize)
		return OnQueueRequest(pcReq, rInfo, EResult::Overflow);

	auto serializer = pcReq->GetSerializer();
	CHAT_ASSERT(serializer, something_went_wrong_ERROR_MESSAGE);
	std::string strRequest = serializer->ToString();
	CHAT_ASSERT(strRequest.size() >= sizeof(ERequestType), 
		"Serializer of Request must always return a string containing at least ERequestType");

	std::pair<std::string, RequestInfo> para(std::move(strRequest), rInfo);
	m_sendRequestMutex.lock();
	m_sendRequestsQueue.push(std::move(para));
	m_sendRequestMutex.unlock();
	return OnQueueRequest(pcReq, rInfo, EResult::Success);
}

EResult ChatSocket::HandleRequest(const Request* pcReq, RequestInfo rInfo)
{
	auto result = OnPreHandleRequest(pcReq, rInfo);
	if(result != EResult::Success)
		return OnHandleRequest(pcReq, rInfo, result);

	CHAT_ASSERT(IsRunning(), "Function supposed to work while connection is running");
	CHAT_EXPECT(pcReq, "Function called with (Request*) pcReq == nullptr");
	CHAT_EXPECT(pcReq->GetType() != ERequestType::Unknown, 
		"Function supposed to work with known request");
	if(!pcReq || pcReq->GetType() == ERequestType::Unknown)
		return OnHandleRequest(pcReq, rInfo, EResult::InvalidParam);

	switch(pcReq->GetType())
	{
		case ERequestType::ClientInfo:
		//TODO FIXME
		case ERequestType::Message:
		//TODO FIXME
		default:
			break;
	}	
	return OnHandleRequest(pcReq, rInfo, EResult::Success);
}

void ChatSocket::ConnectionThreadFunction()
{
    while(!m_bGoingExit)
    {
		PollConnectionChanges();
		PollIncomingRequests();
		PollQueuedRequests();
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

	


} /*END OF NAMESPACES*/