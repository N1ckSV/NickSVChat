
#include <iostream>

#include <steam/steamnetworkingsockets.h>

#include "NickSV/Tools/Utils.h"

#include "NickSV/Chat/ChatSocket.h"

namespace NickSV {
namespace Chat {

EResult ChatSocket::Run(const ChatIPAddr &)
{
	CHAT_ASSERT(!IsRunning(), "Connection is already running");
	CHAT_EXPECT(!m_pConnectionThread, "m_pConnectionThread is expected to be nullptr");
	CHAT_EXPECT(!m_pRequestThread, "m_pRequestThread is expected to be nullptr");
	m_sendRequestsQueue.EnablePush();
	m_handleRequestsQueue.EnablePush();
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
	m_sendRequestsQueue.DisablePush();
	m_handleRequestsQueue.DisablePush();
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
    : m_bGoingExit(true),
      m_pConnectionThread(nullptr),
      m_pRequestThread(nullptr) {};

ChatSocket::~ChatSocket()
{
    delete m_pConnectionThread;
	delete m_pRequestThread;
}

EResult ChatSocket::HandleRequest(ClientInfoRequest&, RequestInfo) 				 { return EResult::Success; };
EResult ChatSocket::HandleRequest(MessageRequest&, RequestInfo) 			 	 { return EResult::Success; };

EResult ChatSocket::OnPreRun(const ChatIPAddr &, ChatErrorMsg &) 				 { return EResult::Success; };
EResult ChatSocket::OnPreQueueRequest(Request&, RequestInfo&)				 	 { return EResult::Success; };
EResult ChatSocket::OnPreHandleRequest(Request&, RequestInfo&)			 		 { return EResult::Success; };
void 	ChatSocket::OnPreCloseSocket() 														  	 	 	   {};

void 	ChatSocket::OnRun(const ChatIPAddr &, EResult , ChatErrorMsg &) 	   							   {};
void 	ChatSocket::OnCloseSocket()    																 	   {};
void 	ChatSocket::OnQueueRequest(const Request&, RequestInfo, EResult)	  		   					   {};
void 	ChatSocket::OnHandleRequest(const Request&, RequestInfo, EResult) 							 	   {};
void    ChatSocket::OnConnectionStatusChanged(ConnectionInfo*, EResult)	 							 	   {};
void    ChatSocket::OnErrorSendingRequest(std::string, RequestInfo, EResult)	 					 	   {};
void    ChatSocket::OnFatalError(const std::string&)	 											 	   {};


void ChatSocket::FatalError(const std::string& errorMsg)
{
	OnFatalError(errorMsg);

	//FIXME TODO Add things here

	CloseSocket();
}

EResult ChatSocket::SendStringToConnection(HSteamNetConnection conn, const std::string& rStr)
{
	SteamNetConnectionRealTimeStatus_t status;
	auto gnsResult = m_pInterface->GetConnectionRealTimeStatus(conn, &status, 0, nullptr);
	if((gnsResult == k_EResultNoConnection) || (status.m_eState != k_ESteamNetworkingConnectionState_Connected))
		return EResult::InvalidConnection;

	CHAT_ASSERT(gnsResult != k_EResultInvalidParam, "Params was not set to be invalid");

	gnsResult = m_pInterface->SendMessageToConnection(conn, rStr.data(), static_cast<uint32>(rStr.size()), 
		k_nSteamNetworkingSend_Reliable, nullptr);
	switch (gnsResult)
	{
	case k_EResultInvalidParam : return EResult::InvalidParam;
	case k_EResultNoConnection :
	case k_EResultInvalidState : return EResult::InvalidConnection;
	case k_EResultLimitExceeded: return EResult::Overflow;
	//case k_EResultIgnored: ...
	default: return EResult::Success;
	}
}



TaskInfo ChatSocket::QueueRequest(Request& rReq, RequestInfo rInfo)
{
	auto result = OnPreQueueRequest(rReq, rInfo);
	if(result != EResult::Success) {
		OnQueueRequest(rReq, rInfo, result); 
		return TaskInfo{ result, Tools::MakeReadyFuture(EResult::NoAction)}; }

	CHAT_EXPECT(rReq.Type() != ERequestType::Unknown, 
		"Function supposed to work with known request");

	if(rReq.Type() == ERequestType::Unknown) {
		OnQueueRequest(rReq, rInfo, EResult::InvalidParam); 
		return TaskInfo{ EResult::InvalidParam, Tools::MakeReadyFuture(EResult::NoAction)}; }

	CHAT_EXPECT(m_sendRequestsQueue.Size() < Constant::MaxSendRequestsQueueSize, 
		"Function called but queue is overflowed");
	if(m_sendRequestsQueue.Size() >= Constant::MaxSendRequestsQueueSize) {
		OnQueueRequest(rReq, rInfo, EResult::Overflow); 
		return TaskInfo{ EResult::Overflow, Tools::MakeReadyFuture(EResult::NoAction)}; }

	std::string strRequest;
	auto serializeResult = rReq.SerializeToString(&strRequest);
	CHAT_ASSERT(serializeResult, "Failed to serialize Request");
	std::promise<EResult> sendResultPromise;
	std::future<EResult>  sendResultFuture = sendResultPromise.get_future();
	if(!m_sendRequestsQueue.Push( { std::move(strRequest), rInfo, std::move(sendResultPromise) } )) {
		OnQueueRequest(rReq, rInfo, EResult::ClosedSocket);
		return TaskInfo{ EResult::ClosedSocket, Tools::MakeReadyFuture(EResult::NoAction) }; }

	OnQueueRequest(rReq, rInfo, EResult::Success);
	return TaskInfo{ EResult::Success, std::move(sendResultFuture) };
}

EResult ChatSocket::SendRequest( Request& rReq, RequestInfo reqInfo)
{
	return QueueRequest(rReq, reqInfo).GetInitialResult();
}

EResult ChatSocket::HandleRequest(Request& rReq, RequestInfo rInfo)
{
	auto result = OnPreHandleRequest(rReq, rInfo);
	if(result != EResult::Success){
		OnHandleRequest(rReq, rInfo, result); 
		return result;}

	CHAT_EXPECT(rReq.Type() != ERequestType::Unknown, "This function supposed to work with known request");
	if(rReq.Type() == ERequestType::Unknown){
		OnHandleRequest(rReq, rInfo, EResult::InvalidParam);
		return EResult::InvalidParam;}

	if(rReq.Type() == ERequestType::ClientInfo)
		result = HandleRequest(static_cast<ClientInfoRequest&>(rReq), rInfo);
	else if (rReq.Type() == ERequestType::Message)
		result = HandleRequest(static_cast<MessageRequest&>(rReq), rInfo);
	
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
	Request request;
	RequestInfo rInfo;
	std::string strReq;
	HandleRequestsQueue_t::ValueType tuple;
    while(!m_bGoingExit)
    {
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		if(m_handleRequestsQueue.IsEmpty())
			continue;
		
		while(!m_handleRequestsQueue.IsEmpty())
		{
			tuple  = m_handleRequestsQueue.Pop();
			strReq = std::move(std::get<0>(tuple));
			rInfo  = std::move(std::get<1>(tuple));
			auto result = request.ParseFromString(strReq);
			CHAT_ASSERT(result, "Failed to parse request");
			HandleRequest(request, rInfo);
		}
    }
}


}}  /*END OF NAMESPACES*/