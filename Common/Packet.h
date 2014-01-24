/**
 * Packet.h
 * Author: Jan Kleszczyñski
 */

#pragma once

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <deque>
#include <sstream>


/** Class of packet representing data that are sent between client and server. */
class Packet {
public:
	/// Const header length
	enum {
		header_length = 16
	};
	/// Max body lengt
	enum {
		max_body_length = 512
	};
	/// Types of message
	enum MsgType {
		MT_NONE = 0,
		MT_DISCONNECT,
		MT_SET_ID,
		MT_SET_NAME,
		MT_BCAST,
		MT_UCAST,
		MT_GET_USERS,
		MT_GET_GAMES,
		MT_BEGIN_GAME,
		MT_END_GAME,
		MT_COUNT
	};
	/// Type of message
	MsgType type;
	/// Sender id
	unsigned int senderId;
	/// Reciver id
	unsigned int reciverId;

	std::stringstream* stream;
	boost::archive::text_oarchive* oarch;
	boost::archive::text_iarchive* iarch;

	Packet() :
			body_length_(0), senderId(0), reciverId(0), type(Packet::MT_NONE), stream(
					NULL) {
		oarch = NULL;
		iarch = NULL;
	}

	/// Returns all packet data
	const char* data() const {
		return data_;
	}
	/// Returns all packet data
	char* data() {
		return data_;
	}
	/// Returns total length
	size_t length() const {
		return header_length + body_length_;
	}
	/// Returns body of packet
	const char* body() const {
		return data_ + header_length;
	}
	/// Returns body of packet
	char* body() {
		return data_ + header_length;
	}
	/// Returns body length
	size_t body_length() const {
		return body_length_;
	}
	/// Sets body length
	void body_length(size_t length) {
		body_length_ = length;
		if (body_length_ > max_body_length)
			body_length_ = max_body_length;
	}
	/// Decode header form data to fields
	bool decode_header() {
		using namespace std;
		// For strncat and atoi.
		char header[header_length + 1] = "";
		strncat(header, data_, header_length);
		char ctype[4] = "";
		char csid[4] = "";
		char crid[4] = "";
		char clength[4] = "";
		strncpy(ctype, header, 4);
		strncpy(csid, header + 4, 4);
		strncpy(crid, header + 8, 4);
		strncpy(clength, header + 12, 4);
		type = (MsgType) atoi(ctype);
		senderId = atoi(csid);
		reciverId = atoi(crid);
		body_length_ = atoi(clength);
		if (body_length_ > max_body_length) {
			body_length_ = 0;
			return false;
		}
		return true;
	}
	/// Encode header form fields to data
	void encode_header() {
		using namespace std;
		// For sprintf and memcpy.
		char header[header_length + 1] = "";
		sprintf(header, "%4d", (unsigned int) type);
		sprintf(header + 4, "%4d", senderId);
		sprintf(header + 8, "%4d", reciverId);
		sprintf(header + 12, "%4d", body_length_);
		memcpy(data_, header, header_length);
	}
	/// Enable writing operations on archive
	void PreaperToWrite() {
		if (oarch != NULL) {
			delete oarch;
			oarch = NULL;
		}
		if (stream != NULL) {
			delete stream;
			stream = NULL;
		}
		stream = new std::stringstream("");

		oarch = new boost::archive::text_oarchive(*stream);
	}
	/// Enable reading operations on archive
	void PreaperToRead() {
		if (iarch != NULL) {
			delete iarch;
			iarch = NULL;
		}
		if (stream != NULL) {
			delete stream;
			stream = NULL;
		}

		if (body_length_ == 0) {
			return;
		}
		stream = new std::stringstream(body());
		iarch = new boost::archive::text_iarchive(*stream);

	}
	/// Writes data to archive
	template<class T> void Write(T& serializable_object) {
		*oarch << serializable_object;
	}
	/// Read data from archive
	template<class T> void Read(T& serializable_object) {
		*iarch >> serializable_object;
	}
	/// Copy archive as body and encode header
	void PrepareToSend() {
		if (stream != NULL) {
			std::string str = (*stream).str();
			body_length(str.size());
			using namespace std;
			strncpy(body(), str.c_str(), body_length());
		}
		encode_header();
	}

private:
	/// Packet data
	char data_[header_length + max_body_length];
	/// Body length
	size_t body_length_;

};

typedef std::deque<Packet> PacketQueue;

