#include "package.h"

#include <string>
#include <algorithm>
#include <iostream>

void printPackage(Package const & package){
	std::cout << package.mPackageNr << ", " << package.mType << ": " << std::string(package.mMessage) << std::endl;
}