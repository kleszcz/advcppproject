/*
 * main.cpp
 *
 *  Created on: May 29, 2012
 *  Author: Pawel Piecyk
 */

#include <iostream>
#include <sqlite3.h>
#include <string>
#include <sstream>
#include <vector>
#include <boost/thread.hpp>

#include "DBProvider.h"
#include "GameServer.h"
#include "TcpMultiSessionServer.h"
#include "../Common/Datamodel.h"

using namespace std;

/**
 * Refreshes data from db with 5s interval
 */
void refreshData(DBProvider *provider) {
	while(true) {
		provider->fetchAllUsers();
		provider->fetchOnlineUsers();
		provider->fetchGames();
		boost::this_thread::sleep(boost::posix_time::milliseconds(5000));
	}
}

/**
 * Handles user input on server
 */
void processUserInput(DBProvider *provider) {

	string command;

	while(command!="quit" && command!="q")
	{
		cout << "\n# ";
		cin >> command;

		if(command=="list") {
			provider->printUsers(ONLINE);
		}else if(command=="games") {
			provider->printGames();
		}else if(command=="list_all") {
			provider->printUsers(ALL);
		}
	}

}

/**
 * Handles messages from clients
 */
void processMessages(DBProvider *provider) {
	boost::asio::io_service io_service;
	boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), 11111);
	PtrTcpMultiSession server(new GameServer(io_service, endpoint, provider));
	io_service.run();
}

int main()
{
	cout << "Welcome to AdvCppGameServerProject" << endl << "Commands: \nlist - prints online users list\nlist_all - prints all users\n" <<
			"games - prints games list\nquit - quits server";

	DBProvider * provider = new DBProvider("db.sqlite");

	// 3 threads with various functionality
	boost::thread messageProcessorThread(processMessages, provider);
	boost::thread dataRefresherThread(refreshData, provider);
	boost::thread userInputThread(processUserInput, provider);

	//we are waiting for quit command
	userInputThread.join();

	//after quit command lets interrupt remaining threads and shutdown the application
	dataRefresherThread.interrupt();
	messageProcessorThread.interrupt();

	cout << "\nShutting down..." << endl;
}



