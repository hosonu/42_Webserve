#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "ServerConfig.hpp"
#include <cstring>
#include <iostream>
#include <fstream>

class Config {
	public:
		Config();
		~Config();

		bool	parse(const std::string &filePath);
	private:
		std::vector<SeverConfig> Servers;
};

#endif
