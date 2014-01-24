/*
 * DBProvider.cpp
 *
 *  Created on: Sep 7, 2012
 *      Author: piotrek
 */

#include <iostream>
#include <cstdio>
#include <sstream>
#include <boost/lexical_cast.hpp>
#include "DBProvider.h"

using namespace std;

DBProvider::DBProvider(const char* dbName) {
	db.open(dbName);
	try {
		stringstream query;
		query << "UPDATE users SET online=0 AND current_game=0";
		db.execDML(query.str().c_str());
	}catch(CppSQLite3Exception &e) {
		cout << e.errorMessage() << endl;
	}
}

DBProvider::~DBProvider() {
	db.close();
}

/**
 * Login user with given username
 * @param username
 */
void DBProvider::loginUser(string username) {
	try {
		stringstream query;
		query << "UPDATE users SET online=1 WHERE login='" << username << "'";
		db.execDML(query.str().c_str());
	}catch(CppSQLite3Exception &e) {
		cout << e.errorMessage() << endl;
	}
}

/**
 * Logout user with given username
 * @param username
 */
void DBProvider::logoutUser(string username) {
	try {
		stringstream query;
		query << "UPDATE users SET online=0 WHERE login='" << username << "'";
		db.execDML(query.str().c_str());
	}catch(CppSQLite3Exception &e) {
		cout << e.errorMessage() << endl;
	}
}

/**
 * Adds information about actually playing game
 * @param username
 * @param gameID - identifier of current game
 */
void DBProvider::beginGame(std::string username, int gameID) {
	try {
		stringstream query;
		query << "UPDATE users SET current_game=" << gameID << " WHERE login='" << username << "'";
		db.execDML(query.str().c_str());
	}catch(CppSQLite3Exception &e) {
		cout << e.errorMessage() << endl;
	}
}

/**
 * Removes information about playing any game from user
 * @param username
 */
void DBProvider::endGame(std::string username) {
	try {
		stringstream query;
		query << "UPDATE users SET current_game=0 WHERE login='" << username << "'";
		db.execDML(query.str().c_str());
	}catch(CppSQLite3Exception &e) {
		cout << e.errorMessage() << endl;
	}
}

/**
 * Fetches all users to usersTable variable
 */
void DBProvider::fetchAllUsers() {
	try {
		usersTable = db.getTable("SELECT * FROM users");
	}catch(CppSQLite3Exception &e) {
		cout << e.errorMessage() << endl;
	}
}

/**
 * Fetches only online users to onlineUsersTable variable
 */
void DBProvider::fetchOnlineUsers() {
	try {
		onlineUsersTable = db.getTable("SELECT * FROM users LEFT JOIN games ON current_game=games.id WHERE users.online=1");
	}catch(CppSQLite3Exception &e) {
		cout << e.errorMessage() << endl;
	}
}

/**
 * Fetches games to gamesTable
 */
void DBProvider::fetchGames() {
	try {
		gamesTable = db.getTable("SELECT * FROM games");
	}catch(CppSQLite3Exception &e) {
		cout << e.errorMessage() << endl;
	}
}

/**
 * Prints users to stadard output
 * @param uStatus - it's enum with possible values: ONLINE, ALL
 */
void DBProvider::printUsers(userStatus uStatus) {

	CppSQLite3Table *table;

	if(uStatus==ONLINE) {
		cout << "Logged in users: " << endl;
		table = &onlineUsersTable;
	}
	else {
		cout << "Users in database: " << endl;
		table = &usersTable;
	}

	try {
		if (table->numRows()!=0) {
			for (int row = 0; row < table->numRows(); row++) {
				table->setRow(row);
				cout << table->fieldValue("login") << " (" << table->fieldValue("first_name")
						<< " " << table->fieldValue("second_name") << ")";

				if(uStatus==ONLINE) {
					if(!table->fieldIsNull("name")) cout << " is playing "
							<< table->fieldValue("name") << endl;
					else cout << " is online" << endl;
				}else { cout << endl; }
			}
		}else {
			cout << "No users" << endl;
		}

	}catch(CppSQLite3Exception &e) {
		cout << e.errorMessage() << endl;
	}
}

/**
 * Prints games to standard output
 */
void DBProvider::printGames() {
	for (int row = 0; row < gamesTable.numRows(); row++) {
		gamesTable.setRow(row);
		cout << "[" << gamesTable.fieldValue("id") << "] " << gamesTable.fieldValue("name") << endl;
	}
}

/**
 * Returns std::list of games (Game object from datamodel - Datamodel.h)
 *
 */
std::list<Game>& DBProvider::getGamesList()
{
	gamesList.clear();

	for (int row = 0; row < gamesTable.numRows(); row++) {
		gamesTable.setRow(row);
		//cout << "[" << gamesTable.fieldValue("id") << "] " << gamesTable.fieldValue("name") << endl;
		gamesList.push_back( Game(gamesTable.fieldValue("name"), boost::lexical_cast<int>(gamesTable.fieldValue("id") ) ) );
	}
	return gamesList;
}

/**
 * Returns std::list of users (User object from datamodel - Datamodel.h)
 */
std::list<User>& DBProvider::getUsersList() {
	usersList.clear();

	for (int row = 0; row < onlineUsersTable.numRows(); row++) {
		onlineUsersTable.setRow(row);
		//cout << "[" << onlineUsersTable.fieldValue("id") << "] " << onlineUsersTable.fieldValue("login") << " "
		//<< onlineUsersTable.fieldValue("name") << endl;
		usersList.push_back( User(onlineUsersTable.fieldValue("login"), onlineUsersTable.fieldValue("first_name"),
				onlineUsersTable.fieldValue("second_name"),
				onlineUsersTable.fieldValue("name"), boost::lexical_cast<int>(onlineUsersTable.fieldValue("id")) ) );
	}
	return usersList;
}







