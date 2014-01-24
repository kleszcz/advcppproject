/**
 * TcpMultiSessionServer.h
 * Author: Jan Kleszczyñski
 */
#pragma once
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <exception>

#include "../Common/Packet.h"

#include "BaseSession.h"

/// Custom exception class
class MyException : public std::exception
{
	public:
		MyException(const std::string& sWhat):msWhat(sWhat)
		{
		}

		virtual ~MyException() throw()
		{}

		std::string msWhat;
		virtual const char* what() const throw() 
		{
			return msWhat.c_str();
		}
};


typedef std::map<unsigned int, PtrBaseSession> SessionsMap;

/**
 * Class of multi sessions server over TCP prootocol. Server handles communication between sessions and connecting new clients (by creating new sessions for them).
 */
class TcpMultiSessionServer
{
public:
	TcpMultiSessionServer(boost::asio::io_service& io_service, const boost::asio::ip::tcp::endpoint& endpoint);

	virtual ~TcpMultiSessionServer(void);

public:
	/// Session joins server
	void Join(PtrBaseSession pSession);
	/// Session leaves server
	void Leave(PtrBaseSession pSession);
	/// Broadcast packet (except session that sent packet)
	void Bcast(const Packet& packet);
	/// Send packet to specific session
	void Ucast(const Packet& packet, unsigned int uiRecvId);
	/// Actions to do when sessions get a global packet (like games list)
	virtual void OnRecv(Packet& packet);
	/// Deprecated. Does nothing
	virtual void OnSend(const Packet& packet);

private:
	/// Handle new client connection and creates session for it.
	void HandleAccept(PtrBaseSession pSession, const boost::system::error_code& error);

private:
	boost::asio::io_service& ioService;
	boost::asio::ip::tcp::acceptor acceptor;
	/// All sessions
	SessionsMap sessions;
	unsigned int lastId;

};
typedef boost::shared_ptr<TcpMultiSessionServer> PtrTcpMultiSession;
