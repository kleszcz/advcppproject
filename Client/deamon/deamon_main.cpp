#include <boost/interprocess/managed_shared_memory.hpp>
#include <cstdlib>
#include <cstddef>
#include <cassert>
#include <utility>
#include <iostream>
#include <boost/thread/thread.hpp>


#include "../package.h"
#include "../interprocessReadWrite.h"



using boost::asio::ip::tcp;
using namespace boost::interprocess;
/*
///http://www.itp.uzh.ch/~dpotter/howto/daemonize
void deamonize(){
    pid_t pid, sid;

    ///already a deamon
    if ( getppid() == 1 ) return;

    ///Fork off the parent process
    pid = fork();
    if (pid < 0) {
        exit(EXIT_FAILURE);
    }
    ///If we got a good PID, then we can exit the parent process.
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    ///At this point we are executing as the child process
    ///Change the file mode mask
    umask(0);

    ///Create a new SID for the child process
    sid = setsid();
    if (sid < 0) {
        exit(EXIT_FAILURE);
    }

    ///Change the current working directory.  This prevents the current
    ///   directory from being locked; hence not being able to remove it.
    if ((chdir("/")) < 0) {
        exit(EXIT_FAILURE);
    }

	///Redirect standard files to /dev/null
    freopen( "/dev/null", "r", stdin);
    freopen( "/dev/null", "w", stdout);
    freopen( "/dev/null", "w", stderr);
}
*/
int main(int argc, char *argv[]){

	///uncomment if you want to create a deamon
	///deamonize();
	
	/// structure to remove share memory
	struct shm_remove{
		shm_remove() { shared_memory_object::remove("SharedMemory"); }
		~shm_remove(){ shared_memory_object::remove("SharedMemory"); }
	} remover;

	try{

		boost::scoped_ptr<mySharedMemoryManager<Package> > smm(new mySharedMemoryManager<Package>(create_only, "SharedMemory") );
		InterprocessReadWrite<Package> irw(smm, "deamon", "gui");
		
		Package input(1,PT_NONE);
		
		boost::asio::io_service io_service;

		tcp::resolver resolver(io_service);
		///change it if server is not on localhost
		tcp::resolver::query query("localhost", "11111");
		tcp::resolver::iterator iterator = resolver.resolve(query);

		Client c(io_service, iterator, &irw);

		boost::thread t(boost::bind(&boost::asio::io_service::run, &io_service));

		irw.readDeamon(input,c);
	
		c.close();
		t.join();
	}
	catch (boost::interprocess::interprocess_exception& e){
		std::cerr << "Interprocess Exception: " << e.what() << "\n";
	}
	catch (std::exception& e){
		std::cerr << "Exception: " << e.what() << "\n";
	}
	
}
