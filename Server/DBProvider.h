/*
 * DBProvider.h
 *
 *  Created on: May 29, 2012
 *  Author: Pawel Piecyk
 */

#ifndef SERVER_H_
#define SERVER_H_

#include <sqlite3.h>
#include <string>
#include "CppSQLite3.h"
#include "../Common/Datamodel.h"
#include <list>

enum userStatus {
	ONLINE = 1,
	ALL = 0
};

/**
 * Database access provider class
 * Provides methods to operate on database
 */
class DBProvider {
public:
	DBProvider(const char* dbName);
	virtual ~DBProvider();

	void loginUser(std::string username);
	void logoutUser(std::string username);

	void beginGame(std::string username, int gameID);
	void endGame(std::string username);

	void fetchAllUsers();
	void fetchOnlineUsers();
	void fetchGames();

	void printUsers(userStatus uStatus);
	void printGames();

	std::list<Game>& getGamesList();
	std::list<User>& getUsersList();

private:
	CppSQLite3DB db;

	//table with all users
	CppSQLite3Table usersTable;

	//table with online users
	CppSQLite3Table onlineUsersTable;

	//table with games
	CppSQLite3Table gamesTable;

	//lists of users ang games necessary for serialization of data
	std::list<Game> gamesList;
	std::list<User> usersList;

};

#endif /* SERVER_H_ */
