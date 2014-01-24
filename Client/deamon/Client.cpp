#include "../Client.h"
#include "../interprocessReadWrite.h"

void Client::OnRecv(Packet& packet){

	if(packet.type == Packet::MT_SET_ID || packet.type == Packet::MT_SET_NAME)
		return;

	if(mIRW != NULL){
		mIRW->onRecv(packet);
	}
	else{
		switch(packet.type)
		{
		//receive and display games list
		case Packet::MT_GET_GAMES:
		{
		packet.PreaperToRead();
		std::list<Game> games;
		packet.Read(games);

		//writeListOfGames(games);
		}
		break;

		case Packet::MT_GET_USERS:
		{
		std::cout << "USERS:" << std::endl;
		//reading users
		packet.PreaperToRead();
		std::list<User> users;
		packet.Read(users);

		//writeListOfUsers(users);
		}
		break;
		}
	}
}