#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "ServerConfig.hpp"
#include "Config_utils.hpp"
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>

template <typename T>
std::string customToString(const T& value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

class Config {
	public:
		Config();
		~Config();

		bool	parse(const std::string &filePath);
		std::vector<ServerConfig> getServerConfig() const;

	private:
		bool checkFileStruct(std::stringstream &file, const std::string &filePath);
		void	checkServerConfigs(const std::vector<ServerConfig>& servers, const std::string &filePath);

		std::vector<ServerConfig> Servers;
};

#endif
