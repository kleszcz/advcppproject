/**
 * Session.h
 * Author: Jan Kleszczyñski
 */

#pragma once
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <deque>

#include "BaseSession.h"
#include "TcpMultiSessionServer.h"
#include "../Common/Packet.h"

/**
 * Class that implements communication between single client and server. */

class Session :
	public BaseSession,
	public boost::enable_shared_from_this<Session>

{
public:

	Session(boost::asio::io_service& io_service, TcpMultiSessionServer& tcpServer) :
			socket(io_service),
			server(tcpServer)
	{		
	}

	~Session(void)
	{
	}

	/// Method that initialize and start session.
	void Start()
	{
		server.Join(shared_from_this());
		#ifndef _SILENT
		std::cout << "Joined to room with session id: " << GetId() << "\n";
		std::cout << "Sending id to client...\n";
		#endif
		Packet msg;
		msg.type = Packet::MT_SET_ID;
		msg.reciverId = id;
		msg.senderId = id;
		msg.PrepareToSend();
		Deliver(msg);
		WaitForHeader();
	}

	boost::asio::ip::tcp::socket& Socket()
	{
		return socket;
	}
	/// Send data to client
	virtual void Deliver(const Packet& packet)
	{
		bool bWriteInProgress = !msgQueue.empty();
		msgQueue.push_back(packet);
		if (!bWriteInProgress)
		{
		  boost::asio::async_write(socket,
			  boost::asio::buffer(msgQueue.front().data(),
				msgQueue.front().length()),
			  boost::bind(&Session::HandleWrite, shared_from_this(),
				boost::asio::placeholders::error));
		}
	}
	/// Listen on socket for header of a packet.
	void WaitForHeader()
	{
		boost::asio::async_read (
			socket,
			boost::asio::buffer (
				readPacket.data(), 
				Packet::header_length ),
			boost::bind(
				&Session::HandleReadHeader, 
				shared_from_this(),
				boost::asio::placeholders::error)
			);
	}


	/// Handle recive of a header and starts listenig for packets body
	void HandleReadHeader(const boost::system::error_code& error)
	{
		if (!error && readPacket.decode_header())
		{					
			boost::asio::async_read(socket,
				boost::asio::buffer(readPacket.body(), readPacket.body_length()),
				boost::bind(&Session::HandleReadBody, shared_from_this(),
				boost::asio::placeholders::error));
		}
		else
		{
			server.Leave(shared_from_this());
		}
	}

	/// Handele recive of a body
	void HandleReadBody(const boost::system::error_code& error)
	{
		if (!error)
		{

			readPacket.PreaperToRead();
			switch(readPacket.type)
			{
			case Packet::MT_SET_NAME:
				readPacket.Read(name);
				#ifndef _SILENT
				std::cout << "Client with id: " << readPacket.senderId << " has name: " << name << std::endl;
				#endif
				break;
			case Packet::MT_BCAST:
				server.Bcast(readPacket);
				break;
			case Packet::MT_UCAST:
				server.Ucast(readPacket, readPacket.reciverId);
				break;
			case Packet::MT_DISCONNECT:
				server.Leave(shared_from_this());
			}			
			server.OnRecv(readPacket);
			WaitForHeader();
		}
		else
		{
			server.Leave(shared_from_this());
		}
	}
	/// Handle sending data to client
	void HandleWrite(const boost::system::error_code& error)
	{
		if (!error)
		{

			msgQueue.pop_front();
			if (!msgQueue.empty())
			{
				boost::asio::async_write(
					socket,
					boost::asio::buffer(
						msgQueue.front().data(),
						msgQueue.front().length()),
						boost::bind(&Session::HandleWrite, shared_from_this(),
					  boost::asio::placeholders::error));
			  }
		}
		else
		{
			server.Leave(shared_from_this());
		}
	}


private:
	/// Sessions socket
	boost::asio::ip::tcp::socket socket;
	/// Ref to server object (for global actions)
	TcpMultiSessionServer& server;
	/// Readed packet
	Packet readPacket;
	/// Packets waiting for being sent
	PacketQueue msgQueue;
};

typedef boost::shared_ptr<Session> PtrSession;
