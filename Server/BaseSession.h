/**
 * BaseSession.h
 * Author: Jan Kleszczyñski
 */

#pragma once
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>

#include "../Common/Packet.h"

/** Interface of session. Session handles communication between client and server. */
class BaseSession
{
public:

	BaseSession(void)
	{
	}

	~BaseSession(void)
	{
	}

public:
	/// Setter of sessions id.
	void SetId(unsigned int uiId)
	{
		id = uiId;
	}

	/// Getter of sessions id.
	unsigned int GetId() const
	{
		return id;
	}
	/// Abstract method semding packets to client.
	virtual void Deliver(const Packet& packet) = 0;
	/// Abstract method initializing session.
	virtual void Start() = 0;
	/// Abstarct method returning socket of session.
	virtual boost::asio::ip::tcp::socket& Socket() = 0;

protected:
	/// Session id
	unsigned int id;
	/// Client name
	std::string name;
};

typedef boost::shared_ptr<BaseSession> PtrBaseSession;
