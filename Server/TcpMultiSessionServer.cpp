#include "TcpMultiSessionServer.h"
//#include <exception>
#include "Session.h"
#include <iostream>

TcpMultiSessionServer::~TcpMultiSessionServer(void)
{
}

TcpMultiSessionServer::TcpMultiSessionServer(boost::asio::io_service& io_service, const boost::asio::ip::tcp::endpoint& endpoint):
		ioService(io_service),
		acceptor(io_service, endpoint),
		lastId(0)
		{
			PtrBaseSession newSession(new Session(ioService, *this));
			acceptor.async_accept(newSession->Socket(),
				boost::bind(&TcpMultiSessionServer::HandleAccept, this, newSession,
				boost::asio::placeholders::error));
		}

void TcpMultiSessionServer::Join(PtrBaseSession pSession)	
{
	++lastId;
	sessions[lastId] = pSession;
	pSession->SetId(lastId);
}

void TcpMultiSessionServer::Leave(PtrBaseSession pSession)	
{
	SessionsMap::iterator it;
	it = sessions.find(pSession->GetId());
	if(it != sessions.end())
	{
		sessions.erase(it);
	}
	else
	{
		char desc[128] = {0};
		sprintf(desc, "Deleting unexisting session! Session id: %d", pSession->GetId());
		std::string d(desc);
		throw MyException(d);
	}
	
}

void TcpMultiSessionServer::Bcast(const Packet& packet)	
{
	SessionsMap::iterator it;
	for(it = sessions.begin(); it != sessions.end(); ++it)
	{
		PtrBaseSession pSession = it->second;
		if(pSession->GetId() == packet.senderId)
			continue;
		pSession->Deliver(packet);
	}
	
}

void TcpMultiSessionServer::Ucast(const Packet& packet, unsigned int uiRecvId)	
{
	SessionsMap::iterator it;
	it = sessions.find(uiRecvId);
	if(it != sessions.end())
	{
		it->second->Deliver(packet);
	}
	else
	{
		char desc[128] = {0};
		sprintf(desc, "Can't send packet to session %d. No such session!", uiRecvId);
		std::string d(desc);
		throw MyException(d);
	}
}

void TcpMultiSessionServer::OnRecv(Packet& packet)	
{
	std::cout << "recived in base!!!"<< std::endl;
}

void TcpMultiSessionServer::OnSend(const Packet& packet)	
{
}

void TcpMultiSessionServer::HandleAccept(PtrBaseSession pSession, const boost::system::error_code& error)
{
	if (!error)
    {
      pSession->Start();
	  PtrBaseSession newSession(new Session(ioService, *this));
      acceptor.async_accept(newSession->Socket(),
          boost::bind(&TcpMultiSessionServer::HandleAccept, this, newSession,
            boost::asio::placeholders::error));
    }
}
