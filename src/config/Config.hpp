#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "ServerConfig.hpp"
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>

class Config {
	public:
		Config();
		~Config();

		bool	parse(const std::string &filePath);
		std::vector<ServerConfig> Servers;
	private:

};

#endif
