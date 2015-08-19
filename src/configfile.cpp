#include "configfile.h"

// verbose 0 : silent
// verbose 1 : normal
// verbose 2 : verbose
// verbose 3 : very verbose
int verbose = 1;

namespace ConfigFile
{
	std::map<std::string, std::string> Configurations;

	void Initialize(const std::string& filename)
	{
		#ifdef _WIN32
			std::string filepath = filename.substr(0, filename.rfind("\\") + 1);
		#else
			std::string filepath = filename.substr(0, filename.rfind("/") + 1);
		#endif
		std::size_t pos;
		std::string s, key;
		std::ifstream file;

		Configurations["filepath"] = filepath;

		file.open(filename);
		if(!file)
		{
			std::cerr << "ERROR: File '" << filename << "' could not be opened!" << std::endl;
			return;
		}

		while (std::getline(file, s))
		{
			pos = s.find(" = ");
			key = s.substr(0, pos);
			s.erase(0, pos + 3);

			s.erase(std::remove(s.begin(), s.end(), '\r'), s.end());
			Configurations[key] = s;
		}

		file.close();
	}
}
