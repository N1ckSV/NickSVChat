
#include <iostream>

#include <steam/steamnetworkingsockets.h>

#include "NickSV/Chat/ChatSocket.h"

#include "NickSV/Chat/Requests/Request.h"
#include "NickSV/Chat/ClientInfo.h"

#include "NickSV/Chat/Serializers/ClientInfoRequestSerializer.h"
#include "NickSV/Chat/Serializers/MessageRequestSerializer.h"

#include "NickSV/Chat/Parsers/RequestParser.h"


namespace NickSV {
namespace Chat {

EResult ChatSocket::Run(const ChatIPAddr &)
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

EResult ChatSocket::HandleClientInfoRequest(ClientInfoRequest&, RequestInfo) 	 { return EResult::Success; };
EResult ChatSocket::HandleMessageRequest(MessageRequest&, RequestInfo) 			 { return EResult::Success; };

EResult ChatSocket::OnPreRun(const ChatIPAddr &, ChatErrorMsg &) 				 { return EResult::Success; };
EResult ChatSocket::OnPreQueueRequest(Request&, RequestInfo&)				 	 { return EResult::Success; };
EResult ChatSocket::OnPreHandleRequest(Request&, RequestInfo&)			 		 { return EResult::Success; };
EResult ChatSocket::OnPreHandleClientInfoRequest(ClientInfoRequest&,RequestInfo&){ return EResult::Success; };
EResult ChatSocket::OnPreHandleMessageRequest(MessageRequest&, RequestInfo&)	 { return EResult::Success; };
void 	ChatSocket::OnPreCloseSocket() 														  	 	 	   {};

void 	ChatSocket::OnRun(const ChatIPAddr &, EResult , ChatErrorMsg &) 	   							   {};
void 	ChatSocket::OnCloseSocket()    																 	   {};
void 	ChatSocket::OnQueueRequest(const Request&, RequestInfo, EResult, std::future<EResult>&)			   {};
void 	ChatSocket::OnHandleRequest(const Request&, RequestInfo, EResult) 							 	   {};
void    ChatSocket::OnHandleClientInfoRequest(const ClientInfoRequest&, RequestInfo, EResult)			   {};
void    ChatSocket::OnHandleMessageRequest(const MessageRequest&, RequestInfo, EResult)	   			  	   {};
void    ChatSocket::OnConnectionStatusChanged(ConnectionInfo*, EResult)	 							 	   {};
void    ChatSocket::OnErrorSendingRequest(std::string , RequestInfo, EResult)	 					 	   {};
void    ChatSocket::OnFatalError(const std::string&)	 											 	   {};
bool    ChatSocket::OnValidateClientInfo(const ClientInfo&) 								 { return true; };


bool ChatSocket::ValidateClientInfo(const ClientInfo& rClientInfo)
{
	if(!OnValidateClientInfo(rClientInfo))
		return false;
	
	bool result = rClientInfo.GetUserID() >= Constant::LibReservedUserIDs;
	return result;
}

void ChatSocket::FatalError(const std::string& errorMsg)
{
	OnFatalError(errorMsg);
	CloseSocket();
}

EResult ChatSocket::SendStringToConnection(HSteamNetConnection conn, const std::string& rStr)
{
	auto gnsResult = m_pInterface->SendMessageToConnection(conn, rStr.data(), (uint32)rStr.size(), 
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


EResult ChatSocket::QueueRequest(Request& rReq, RequestInfo rInfo, std::future<EResult>& sendResult)
{
	std::promise<EResult> sendResultPromise;
	sendResult = sendResultPromise.get_future();
	auto result = OnPreQueueRequest(rReq, rInfo);
	if(result != EResult::Success) {
		sendResultPromise.set_value(result);
		OnQueueRequest(rReq, rInfo, result, sendResult); 
		return result; }

	CHAT_EXPECT(rReq.GetType() != ERequestType::Unknown, 
		"Function supposed to work with known request");

	if(rReq.GetType() == ERequestType::Unknown) { 
		sendResultPromise.set_value(EResult::InvalidParam);
		OnQueueRequest(rReq, rInfo, EResult::InvalidParam, sendResult); 
		return EResult::InvalidParam; }

	CHAT_EXPECT(m_sendRequestsQueue.size() < Constant::MaxSendRequestsQueueSize, 
		"Function called but queue is overflowed");
	if(m_sendRequestsQueue.size() >= Constant::MaxSendRequestsQueueSize) { 
		sendResultPromise.set_value(EResult::Overflow);
		OnQueueRequest(rReq, rInfo, EResult::Overflow, sendResult); 
		return EResult::Overflow; }

	auto serializer = rReq.GetSerializer();
	CHAT_ASSERT(serializer, something_went_wrong_ERROR_MESSAGE);
	std::string strRequest = serializer->ToString();
	CHAT_ASSERT(strRequest.size() >= sizeof(ERequestType), 
		"Serializer of Request must always return a string containing at least ERequestType");
	m_Mutexes.sendRequestMutex.lock();
	m_sendRequestsQueue.push({ std::move(strRequest), rInfo, std::move(sendResultPromise) });
	m_Mutexes.sendRequestMutex.unlock();
	OnQueueRequest(rReq, rInfo, EResult::Success, sendResult);
	return EResult::Success;
}

EResult ChatSocket::QueueRequest(Request& rReq, RequestInfo rInfo)
{
	std::future<EResult> _;
	return QueueRequest(rReq, rInfo, _);
}

EResult ChatSocket::HandleRequest(Request& rReq, RequestInfo rInfo)
{
	auto result = OnPreHandleRequest(rReq, rInfo);
	if(result != EResult::Success){
		OnHandleRequest(rReq, rInfo, result); 
		return result;}

	CHAT_EXPECT(rReq.GetType() != ERequestType::Unknown, "This function supposed to work with known request");
	if(rReq.GetType() == ERequestType::Unknown){
		OnHandleRequest(rReq, rInfo, EResult::InvalidParam);
		return EResult::InvalidParam;}

	switch(rReq.GetType())
	{
	case ERequestType::ClientInfo:
		result = HandleClientInfoRequest(static_cast<ClientInfoRequest&>(rReq), rInfo);
		break;
	case ERequestType::Message:
		result = HandleMessageRequest(static_cast<MessageRequest&>(rReq), rInfo);
		break;
	default: break;
	}	
	OnHandleRequest(rReq, rInfo, result);
	return result;
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
			strReq = std::move(std::get<0>(m_handleRequestsQueue.front()));
			rInfo  = std::get<1>(m_handleRequestsQueue.front());
			m_handleRequestsQueue.pop();
			m_Mutexes.handleRequestMutex.unlock();
			parser.FromString(strReq);
			// HandleRequest returns EResult, but it is only needed for manual calls
			HandleRequest(*parser.GetObject(), rInfo);
		}
    }
}


}}  /*END OF NAMESPACES*/