#pragma once
#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <string>


// verbose == 0 : quiet

// verbose == 1 : normal

// verbose == 2 : verbose

// verbose == 3 : very verbose
extern int verbose;

namespace ConfigFile
{
	extern std::map<std::string, std::string> Configurations;

	void Initialize(const std::string& filename);
}
