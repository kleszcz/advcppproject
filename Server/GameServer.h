#pragma once

#include <iostream>
#include <string>
#include "TcpMultiSessionServer.h"
#include <sstream>

#include "DBProvider.h"
#include "../Common/Datamodel.h"

/**
 * Server class to handle incoming messages
 */
class GameServer: public TcpMultiSessionServer
{
public:

	DBProvider* dbProvider;

	GameServer(boost::asio::io_service& io_service,
			const boost::asio::ip::tcp::endpoint& endpoint, DBProvider* dbProv) :
			TcpMultiSessionServer(io_service, endpoint), dbProvider(dbProv)
	{
	}

	/**
	 * Receives and handles incoming packets
	 * @param packet - incoming packet
	 */
	virtual void OnRecv(Packet& packet)
	{
		switch (packet.type)
		{
		case Packet::MT_SET_ID:
			return;
			break;

		case Packet::MT_SET_NAME: //user login
		{
			packet.PreaperToRead();
			std::string username;
			packet.Read(username);
			dbProvider->loginUser(username);
		}
			break;

		case Packet::MT_GET_GAMES: //get games
		{
			Packet msg;
			msg.reciverId = packet.senderId;
			msg.senderId = packet.reciverId;
			msg.type = Packet::MT_GET_GAMES;
			msg.PreaperToWrite();
			msg.Write(dbProvider->getGamesList());
			msg.PrepareToSend();
			Ucast(msg, packet.senderId);
		}
			break;

		case Packet::MT_GET_USERS: //get users
		{
			Packet msg;
			msg.reciverId = packet.senderId;
			msg.senderId = packet.reciverId;
			msg.type = Packet::MT_GET_USERS;
			msg.PreaperToWrite();
			msg.Write(dbProvider->getUsersList());
			msg.PrepareToSend();
			Ucast(msg, packet.senderId);
		}
			break;

		case Packet::MT_BEGIN_GAME: //begin game
		{
			packet.PreaperToRead();
			int gameId;
			packet.Read(gameId);
			std::string username;
			packet.Read(username);
			dbProvider->beginGame(username, gameId);
		}
			break;

		case Packet::MT_END_GAME: //begin game
		{
			packet.PreaperToRead();
			std::string username;
			packet.Read(username);
			dbProvider->endGame(username);
		}
			break;

		case Packet::MT_DISCONNECT: //disconnect/logout user
		{
			packet.PreaperToRead();
			std::string username;
			packet.Read(username);
			dbProvider->logoutUser(username);
		}
			break;

		default: //just read body to std::string
			packet.PreaperToRead();
			std::string l;
			packet.Read(l);
			break;
		}

	}

};
