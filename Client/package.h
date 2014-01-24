#ifndef _PACKEGE_H_
#define _PACKEGE_H_

#include <string>
#include <algorithm>
#include <stdio.h>
#include <string.h>

///package typ
enum PACKAGE_TYPE{
	PT_NONE = 0,
	PT_GET_USERS,
	PT_GET_GAMES,
	PT_BEGIN_GAME,
	PT_END_GAME,
	PT_STRING_BEGIN,
	PT_STRING,
	PT_STRING_END,
	PT_CONNECT,
	PT_DISCONNECT
};

struct Package{
	#define SIZE 201

	Package(int packageNr, int type): 
		mPackageNr(packageNr), mType(type)
	{ }
	
	Package(int packageNr, int type, std::string const& message): 
		mPackageNr(packageNr), mType(type)
	{
		memcpy(mMessage, message.c_str(), std::min<size_t>(SIZE, message.size()));
	};
	
	Package(const Package &package){
		mPackageNr=package.mPackageNr;
		mType=package.mType;
		mMessageSize = package.mMessageSize;
		memcpy(mMessage, package.mMessage, SIZE);
	}
			 
	Package& operator= (const Package& package){
		if(this == &package){return *this;}
		
		mPackageNr=package.mPackageNr;
		mType=package.mType;
		mMessageSize = package.mMessageSize;
		memcpy(mMessage, package.mMessage, SIZE);
		
		return *this;
	}
	
	/// unuse, plan to synchronization process communication
	unsigned int mPackageNr;
	
	/// type of package
	int mType;
	/// size of massage array
	size_t mMessageSize;
	union{
		char mMessage[SIZE];
		unsigned int mGameIdx;
	};
	
};

void printPackage(Package const & package);

#endif