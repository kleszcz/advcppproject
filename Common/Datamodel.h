#ifndef DATAMODEL_H_
#define DATAMODEL_H_

#include <string>
#include <boost/serialization/list.hpp>

class User {
public:
	std::string login;
	std::string fname;
	std::string sname;
	std::string game;
	int id;

	User(std::string pLogin, std::string pFname, std::string pSname,
			std::string pGame, int pId) :
			login(pLogin), fname(pFname), sname(pSname), game(pGame), id(pId) {
	}
	User() {
	}

	template<typename Archive>
	void serialize(Archive& ar, const unsigned version) {
		ar & login & fname & sname & game & id;
	}
};

class Game {
public:
	Game(std::string pName, int pId) :
			name(pName), id(pId) {
	}
	Game() {
	}
	std::string name;
	int id;

	template<typename Archive>
	void serialize(Archive& ar, const unsigned version) {
		ar & name & id;
	}
};

#endif
