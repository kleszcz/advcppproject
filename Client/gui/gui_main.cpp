#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/scoped_ptr.hpp>
#include <cstdlib>
#include <cstddef>
#include <cassert>
#include <utility>
#include <iostream>

#include "../package.h"
#include "../interprocessReadWrite.h"

template< typename T >
void getUsers(InterprocessReadWrite<T>& irw){
	Package package(1, PT_GET_USERS);
	irw.write(package);

	Package output(1, PT_GET_USERS);
	irw.readGui(output);
}

template< typename T >
void getGames(InterprocessReadWrite<T>& irw){
	Package package(1, PT_GET_GAMES);
	irw.write(package);

	Package output(1, PT_GET_GAMES);
	irw.readGui(output);
}

template< typename T >
void playGame(InterprocessReadWrite<T>& irw){
	std::cout << "Which one?" << std::endl;
	getGames(irw);
	
	unsigned gameIdx;
	std::cin >> gameIdx;
	
	Package output(1, PT_BEGIN_GAME);
	output.mGameIdx = gameIdx;
	
	irw.write(output);
}



int main(int argc, char *argv[]){



	try{

		using namespace boost::interprocess;
					
		boost::scoped_ptr<mySharedMemoryManager<Package> > smm(new mySharedMemoryManager<Package>(open_only, "SharedMemory") );
		InterprocessReadWrite<Package> irw(smm, "gui", "deamon");
		
		Package input(1,PT_NONE);
			 
		irw.write(Package(1, PT_CONNECT, ""));		
		char temp;
		do{
			
			std::cout << "\n1 - online users list \n2 - games list \n3 - play game \n4 - quit \n" << std::endl;
			std::cin >> temp;
			
			if(temp == '1'){getUsers(irw);}
			else if(temp == '2'){getGames(irw);}
			else if(temp == '3'){playGame(irw);}
			
		}while(temp != '4');

		irw.write(Package(1, PT_DISCONNECT));
	}
	catch (boost::interprocess::interprocess_exception& e){
		std::cerr << "Exception: " << e.what() << "\n";
		std::cerr << "execute daemon first\n";
	}

}
