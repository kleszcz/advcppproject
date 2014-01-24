/**
 * Client.h
 * Author: Jan Kleszczyñski & Tomasz Kowalski
 */

#pragma once
#include <cstdlib>

#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include "../Common/Packet.h"
#include "../Common/Datamodel.h"
#include "package.h"

template <typename T>
class InterprocessReadWrite;

/**
 * Class handles all cilent server communication
 */
class Client
{
public:

	std::string name;
	unsigned int id;
	InterprocessReadWrite<Package>* mIRW;

	Client(boost::asio::io_service& io_service,
      boost::asio::ip::tcp::resolver::iterator endpoint_iterator)
    : IoService(io_service),
      Socket(io_service),
	  id(0),
	  mIRW(NULL)
  {
	  name = "nickel"; //it should be loaded from settings (or from command line parameter)
    boost::asio::ip::tcp::endpoint endpoint = *endpoint_iterator;
    Socket.async_connect(endpoint,
        boost::bind(&Client::handle_connect, this,
          boost::asio::placeholders::error, ++endpoint_iterator));
  }
	
	
  Client(boost::asio::io_service& io_service,
      boost::asio::ip::tcp::resolver::iterator endpoint_iterator,
	  InterprocessReadWrite<Package>* irw)
    : IoService(io_service),
      Socket(io_service),
	  id(0),mIRW(irw)
  {
	  name = "nickel"; //it should be loaded from settings (or from command line parameter)
    boost::asio::ip::tcp::endpoint endpoint = *endpoint_iterator;
    Socket.async_connect(endpoint,
        boost::bind(&Client::handle_connect, this,
          boost::asio::placeholders::error, ++endpoint_iterator));
  }
  /// Write packet to server
  void write(const Packet& msg)
  {
    IoService.post(boost::bind(&Client::do_write, this, msg));
  }
  /// Cloe connection
  void close()
  {
    IoService.post(boost::bind(&Client::do_close, this));
  }

protected:
	/// Handles actions after connetct to server
  void handle_connect(const boost::system::error_code& error,
      boost::asio::ip::tcp::resolver::iterator endpoint_iterator)
  {
    if (!error)
    {
      boost::asio::async_read(Socket,
          boost::asio::buffer(read_msg.data(), Packet::header_length),
          boost::bind(&Client::handle_read_header, this,
            boost::asio::placeholders::error));
    }
    else if (endpoint_iterator != boost::asio::ip::tcp::resolver::iterator())
    {
      Socket.close();
      boost::asio::ip::tcp::endpoint endpoint = *endpoint_iterator;
      Socket.async_connect(endpoint,
          boost::bind(&Client::handle_connect, this,
            boost::asio::placeholders::error, ++endpoint_iterator));
    }
  }
  /// Read header of packet and waits for body
  void handle_read_header(const boost::system::error_code& error)
  {
    if (!error && read_msg.decode_header())
    {
      boost::asio::async_read(Socket,
          boost::asio::buffer(read_msg.body(), read_msg.body_length()),
          boost::bind(&Client::handle_read_body, this,
            boost::asio::placeholders::error));
    }
    else
    {
      do_close();
    }
  }
  /// Read body of packet
  void handle_read_body(const boost::system::error_code& error)
  {
    if (!error)
    {
		read_msg.PreaperToRead();
		switch(read_msg.type)
		{
		case Packet::MT_SET_ID:
			id = read_msg.reciverId;
			#ifndef _SLIENT
			std::cout << "Session id: " << id << std::endl;
			#endif
			break;
		}
		OnRecv(read_msg);
      boost::asio::async_read(Socket,
          boost::asio::buffer(read_msg.data(), Packet::header_length),
          boost::bind(&Client::handle_read_header, this,
            boost::asio::placeholders::error));
    }
    else
    {
      do_close();
    }
  }

  /// Secific actions on recived packets
	virtual void OnRecv(Packet& packet);
/// Send data to server
  void do_write(Packet msg)
  {
    bool write_in_progress = !write_msgs.empty();
    write_msgs.push_back(msg);
    if (!write_in_progress)
    {
      boost::asio::async_write(Socket,
          boost::asio::buffer(write_msgs.front().data(),
            write_msgs.front().length()),
          boost::bind(&Client::handle_write, this,
            boost::asio::placeholders::error));
    }
  }
  /// Handles actions after sending data
  void handle_write(const boost::system::error_code& error)
  {
    if (!error)
    {
      write_msgs.pop_front();
      if (!write_msgs.empty())
      {
        boost::asio::async_write(Socket,
            boost::asio::buffer(write_msgs.front().data(),
              write_msgs.front().length()),
            boost::bind(&Client::handle_write, this,
              boost::asio::placeholders::error));
      }
    }
    else
    {
      do_close();
    }
  }
  /// Close client connection
  void do_close()
  {
    Socket.close();
  }

protected:
  boost::asio::io_service& IoService;
  boost::asio::ip::tcp::socket Socket;
  Packet read_msg;
  PacketQueue write_msgs;
};
