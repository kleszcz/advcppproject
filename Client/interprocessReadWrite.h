#ifndef _INTERPROCESS_READ_WRITE_H_
#define _INTERPROCESS_READ_WRITE_H_

#include <string>
#include <sstream>
#include <boost/circular_buffer.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/creation_tags.hpp>
#include <boost/thread/thread.hpp>


#include <list>
#include "package.h"
#include "Client.h"
#include "../Common/Datamodel.h"
#include "../Common/Packet.h"

///template for segment of shared memory
template< typename T> 
class mySharedMemoryManager{
	
	/// name of shared memory
	std::string mName;
	
	///sheard memory
	boost::scoped_ptr<boost::interprocess::managed_shared_memory> mSegment;

	public:
	
		/// constructor to create only
		mySharedMemoryManager(boost::interprocess::create_only_t create_only, std::string const& name):
			mName(name),
			mSegment( new boost::interprocess::managed_shared_memory(create_only, name.c_str(), (3*sizeof(T)) ) )
		{ }
		
		/// constructor to open only
		mySharedMemoryManager(boost::interprocess::open_only_t open_only, std::string const& name):
			mName(name),
			mSegment(new boost::interprocess::managed_shared_memory( open_only, name.c_str()) )
		{ }
					
		/// segmen getter
		boost::interprocess::managed_shared_memory* getSegment()const{
			return mSegment.get();
		}
};


/// template to read write comunication between process
template< typename T> 
class InterprocessReadWrite{

	/// name of input message tag
	std::string mInput;
	/// name of output message tag
	std::string mOutput;
		
	/// shared memory segment
	boost::scoped_ptr< mySharedMemoryManager<T> > & mSMM;

	public:
		
	InterprocessReadWrite(boost::scoped_ptr< mySharedMemoryManager<T> > & smm, std::string const& input, std::string const& output):
		mInput(input), mOutput(output), mSMM(smm)
	{}
		
	/// read if exist in sheard memory elemnt with tag mInput
	bool read(T & element){	
		boost::interprocess::managed_shared_memory* segment = mSMM.get()->getSegment();
		std::pair<T*, size_t> res;
		res = segment->find<T>(mInput.c_str());
		
		if(res.second == 1){	
			element = (*(res.first));
			segment->destroy<T>(mInput.c_str());
			return true;
		}

		return false;
	}
	
	/// read and send next packet to Client
	void readDeamon(T & element, Client & c){
	
		while(true){
			if(read(element)){
				switch(element.mType){
					case PT_GET_USERS:{
						Packet msg;
						msg.type = Packet::MT_GET_USERS;
						msg.reciverId = c.id;
						msg.senderId = c.id;
						msg.PrepareToSend();
						c.write(msg);
						}break;
					case PT_GET_GAMES:{
						Packet msg;
						msg.type = Packet::MT_GET_GAMES;
						msg.reciverId = c.id;
						msg.senderId = c.id;
						msg.PrepareToSend();
						c.write(msg);
						}break;
					case PT_BEGIN_GAME:{
						Packet msg;
						msg.type = Packet::MT_BEGIN_GAME;
						msg.reciverId = c.id;
						msg.senderId = c.id;
						msg.PreaperToWrite();
						msg.Write(element.mGameIdx);
						msg.Write(c.name);
						msg.PrepareToSend();
						c.write(msg);
						}break;
					case PT_CONNECT:{
						Packet msg;
						msg.type = Packet::MT_SET_NAME;
						msg.reciverId = c.id;
						msg.senderId = c.id;
						msg.PreaperToWrite();
						msg.Write(c.name);
						msg.PrepareToSend();
						c.write(msg);
						}break;
					case PT_DISCONNECT:{
						Packet msg;
						msg.type = Packet::MT_DISCONNECT;
						msg.reciverId = c.id;
						msg.senderId = c.id;
						msg.PreaperToWrite();
						msg.Write(c.name);
						msg.PrepareToSend();
						c.write(msg);
						return;
						}break;
				}
			}
			boost::this_thread::sleep( boost::posix_time::milliseconds(100) );
		}
		
	}
	
	/// read until he read a element or elements with multiline-string
	void readGui(T & element){
		enum{WAS_READ_NO_MULTILINE,
			WAS_READ_MULTILINA,
			WASNT_READ
		}readStatus;
				
		do{
						
			readStatus = (read(element)? WAS_READ_NO_MULTILINE : WASNT_READ);
		
			if(element.mType == PT_STRING_BEGIN){ readStatus = WAS_READ_MULTILINA;}
				
			else if(element.mType == PT_STRING){
				readStatus = WAS_READ_MULTILINA;
				std::string temp(element.mMessage,element.mMessageSize);
				//std::cout << temp.size() << ", " << element.mMessageSize;
				std::cout << temp << std::endl;
			}
						
			boost::this_thread::sleep( boost::posix_time::milliseconds(100) );
		}while(readStatus != WAS_READ_NO_MULTILINE);
		
	}
	
	/// write if doesn't exist in sheard memory elemnt with tag mOutput
	bool tryWrite(T const& element){	
		boost::interprocess::managed_shared_memory* segment = mSMM.get()->getSegment();
		
		if(segment->find<T>(mOutput.c_str()).second == 0){
			segment->construct<T>(mOutput.c_str())(element);
			return true;
		}
		return false;
	}
	
	/// wirte until he will succeed 
	void write(T const& element){
		while(!tryWrite(element)){
			boost::this_thread::sleep( boost::posix_time::milliseconds(100) );
		}
	}
	
	/// print info when Client receive data
	void onRecv(Packet& packet){
				
		switch(packet.type){
			///receive and display games list
			case Packet::MT_GET_GAMES:{
			packet.PreaperToRead();
			std::list<Game> games;
			packet.Read(games);

			
			Package package(1, PT_STRING_BEGIN);
			write(package);
			
			while (!games.empty()){
				Game g = games.front();
				
				std::stringstream tempSS;
				std::string tempString;

				tempSS << "[" << g.id << "] " << g.name << "\0";
				tempString=tempSS.str();
				
				//std::cout << "size:" <<tempString.size() << std::endl;

				games.pop_front();
				
				package = Package(1, PT_STRING, tempString);
				package.mMessageSize = tempString.size();
				write(package);
			}
			
			package = Package(1, PT_STRING_END);
			write(package);
			}break;

		case Packet::MT_GET_USERS:{
			///reading users
			packet.PreaperToRead();
			std::list<User> users;
			packet.Read(users);

			Package package(1, PT_STRING_BEGIN);
			write(package);
			
			while (!users.empty()){
				User u = users.front();
				
				std::stringstream tempSS;
				std::string tempString;

				tempSS <<  u.login << "playing now: " << u.game << "\0";
				tempString=tempSS.str();

				users.pop_front();
				
				package = Package(1, PT_STRING, tempString);
				package.mMessageSize = tempString.size();
				write(package);
			}
			
			package = Package(1, PT_STRING_END);
			write(package);
			

			}break;
		}
	
	}	
};


#endif