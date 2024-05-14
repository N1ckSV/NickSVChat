
#include <iostream>

#include <steam/steamnetworkingsockets.h>

#include "NickSV/Chat/ChatSocket.h"

#include "NickSV/Chat/Requests/Request.h"
#include "NickSV/Chat/ClientInfo.h"

#include "NickSV/Chat/Serializers/ClientInfoRequestSerializer.h"
#include "NickSV/Chat/Serializers/MessageRequestSerializer.h"

#include "NickSV/Chat/Parsers/RequestParser.h"


namespace NickSV::Chat {

EResult ChatSocket::Run(const ChatIPAddr &serverAddr)
{
	CHAT_ASSERT(!IsRunning(), "Connection is already running");
	CHAT_EXPECT(!m_pConnectionThread, "m_pConnectionThread is expected to be nullptr");
	CHAT_EXPECT(!m_pRequestThread, "m_pRequestThread is expected to be nullptr");
	delete m_pConnectionThread;
	delete m_pRequestThread;
    return EResult::Success;
}

void ChatSocket::CloseSocket()
{
	if(m_bGoingExit)
		return;
    CHAT_EXPECT(m_pConnectionThread, "Connection thread is missing (m_pConnectionThread is null).");
    CHAT_EXPECT(m_pRequestThread, "Request thread is missing (m_pRequestThread is null).");
    OnPreCloseSocket();
    m_bGoingExit = true;
}

bool inline ChatSocket::IsRunning() { return !m_bGoingExit; }

void ChatSocket::Wait()
{
	// Wait() is not supposed to be within one of the threads it is waiting on
	// (one of ChatSockets owns)
	CHAT_ASSERT(std::this_thread::get_id() != m_pConnectionThread->get_id() && 
	   			std::this_thread::get_id() != m_pRequestThread->get_id(),
				"Wait() function is called within one of the threads it is waiting on");
	if(m_pConnectionThread->joinable())
   		m_pConnectionThread->join();
	if(m_pRequestThread->joinable())
   		m_pRequestThread->join();
}

ChatSocket::ChatSocket()
{
    m_bGoingExit = true;
    m_pConnectionThread = nullptr;
    m_pRequestThread = nullptr;
}

ChatSocket::~ChatSocket()
{
    delete m_pConnectionThread;
	delete m_pRequestThread;
}

void    ChatSocket::HandleClientInfoRequest(ClientInfoRequest*, RequestInfo) 							   {};
void    ChatSocket::HandleMessageRequest(MessageRequest*, RequestInfo) 								 	   {};

EResult ChatSocket::OnPreRun(const ChatIPAddr &serverAddr, ChatErrorMsg &errMsg) { return EResult::Success; };
EResult ChatSocket::OnPreQueueRequest(Request*, RequestInfo&)				 	 { return EResult::Success; };
EResult ChatSocket::OnPreHandleRequest(Request*, RequestInfo&)			 		 { return EResult::Success; };
EResult ChatSocket::OnPreHandleClientInfoRequest(ClientInfoRequest*, RequestInfo){ return EResult::Success; };
EResult ChatSocket::OnPreHandleMessageRequest(MessageRequest*, RequestInfo)	 	 { return EResult::Success; };
void 	ChatSocket::OnPreCloseSocket() 														  	 	 	   {};
void 	ChatSocket::OnRun(const ChatIPAddr &serverAddr, EResult outsideResult, ChatErrorMsg &errMsg) 	   {};
void 	ChatSocket::OnCloseSocket()    																 	   {};
void 	ChatSocket::OnQueueRequest(const Request*, RequestInfo, EResult)				 			 	   {};
void 	ChatSocket::OnHandleRequest(const Request*, RequestInfo, EResult) 							 	   {};
void    ChatSocket::OnHandleClientInfoRequest(const ClientInfoRequest*, RequestInfo, EResult outsideResult){};
void    ChatSocket::OnHandleMessageRequest(const MessageRequest*, RequestInfo, EResult outsideResult)	   {};
void    ChatSocket::OnConnectionStatusChanged(ConnectionInfo*, EResult)	 							 	   {};
void    ChatSocket::OnErrorSendingRequest(std::string , RequestInfo, EResult)	 					 	   {};
void    ChatSocket::OnFatalError(const std::string&)	 											 	   {};
bool    ChatSocket::OnValidateClientInfo(const ClientInfo* pClientInfo) 					 { return true; };


bool ChatSocket::ValidateClientInfo(const ClientInfo* pClientInfo)
{
	if(!OnValidateClientInfo(pClientInfo))
		return false;
	
	bool result = pClientInfo->GetUserID() >= Constant::LibReservedUserIDs;
	return result;
}

void ChatSocket::FatalError(const std::string& errorMsg)
{
	OnFatalError(errorMsg);
	CloseSocket();
}

EResult ChatSocket::SendStringToConnection(HSteamNetConnection conn, const std::string* pStr)
{
	auto gnsResult = m_pInterface->SendMessageToConnection(conn, pStr->data(), (uint32)pStr->size(), 
		k_nSteamNetworkingSend_Reliable, nullptr);
	switch (gnsResult)
	{
	case k_EResultInvalidParam: return EResult::InvalidParam;
	case k_EResultNoConnection:
	case k_EResultInvalidState: return EResult::InvalidConnection;
	case k_EResultLimitExceeded: return EResult::Overflow;
	default: return EResult::Success;
	}
}


std::unique_ptr<ClientInfo> ChatSocket::MakeClientInfo() { return std::make_unique<ClientInfo>(); }

EResult ChatSocket::QueueRequest(Request* pcReq, RequestInfo rInfo)
{
	auto result = OnPreQueueRequest(pcReq, rInfo);
	if(result != EResult::Success) { 
		OnQueueRequest(pcReq, rInfo, result); 
		return result; }

	CHAT_EXPECT(pcReq, "Function called with (Request*) pcReq == nullptr");
	CHAT_EXPECT(pcReq->GetType() != ERequestType::Unknown, 
		"Function supposed to work with known request");

	if(!pcReq || pcReq->GetType() == ERequestType::Unknown) { 
		OnQueueRequest(pcReq, rInfo, EResult::InvalidParam); 
		return EResult::InvalidParam; }

	CHAT_EXPECT(m_sendRequestsQueue.size() < Constant::MaxSendRequestsQueueSize, 
		"Function called but queue is overflowed");
	if(m_sendRequestsQueue.size() >= Constant::MaxSendRequestsQueueSize) { 
		OnQueueRequest(pcReq, rInfo, EResult::Overflow); 
		return EResult::Overflow; }

	auto serializer = pcReq->GetSerializer();
	CHAT_ASSERT(serializer, something_went_wrong_ERROR_MESSAGE);
	std::string strRequest = serializer->ToString();
	CHAT_ASSERT(strRequest.size() >= sizeof(ERequestType), 
		"Serializer of Request must always return a string containing at least ERequestType");
	std::pair<std::string, RequestInfo> para(std::move(strRequest), rInfo);
	m_Mutexes.sendRequestMutex.lock();
	m_sendRequestsQueue.push(std::move(para));
	m_Mutexes.sendRequestMutex.unlock();
	OnQueueRequest(pcReq, rInfo, EResult::Success);
	return EResult::Success;
}

void ChatSocket::HandleRequest(Request* pcReq, RequestInfo rInfo)
{
	auto result = OnPreHandleRequest(pcReq, rInfo);
	if(result != EResult::Success){
		OnHandleRequest(pcReq, rInfo, result);
		return;}

	CHAT_EXPECT(pcReq, "Function called with (Request*) pcReq == nullptr");
	CHAT_EXPECT(pcReq->GetType() != ERequestType::Unknown, "Function supposed to work with known request");
	if(!pcReq || pcReq->GetType() == ERequestType::Unknown){
		OnHandleRequest(pcReq, rInfo, EResult::InvalidParam);
		return;}

	switch(pcReq->GetType())
	{
	case ERequestType::ClientInfo:
		HandleClientInfoRequest(static_cast<ClientInfoRequest*>(pcReq), rInfo);
		break;
	case ERequestType::Message:
		HandleMessageRequest(static_cast<MessageRequest*>(pcReq), rInfo);
		break;
	default:
		break;
	}	
	OnHandleRequest(pcReq, rInfo, EResult::Success);;
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

void ChatSocket::RequestThreadFunction()
{
	RequestInfo rInfo;
	std::string strReq; 
	Parser<Request> parser;
    while(!m_bGoingExit)
    {
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		if(m_handleRequestsQueue.empty())
			continue;
		
		while(!m_handleRequestsQueue.empty())
		{
			m_Mutexes.handleRequestMutex.lock();
			strReq = std::move(m_handleRequestsQueue.front().first);
			rInfo = m_handleRequestsQueue.front().second;
			m_handleRequestsQueue.pop();
			m_Mutexes.handleRequestMutex.unlock();
			parser.FromString(strReq);
			HandleRequest(parser.GetObject().get(), rInfo);
		}
    }
}


} /*END OF NAMESPACES*/