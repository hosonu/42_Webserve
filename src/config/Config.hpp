#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "ServerConfig.hpp"
#include "Config_utils.hpp"
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>

class Config {
	public:
		Config();
		~Config();

		bool	parse(const std::string &filePath);

		std::vector<ServerConfig> getServerConfig() const;

	private:
		std::vector<ServerConfig> Servers;
};

#endif
