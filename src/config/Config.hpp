#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "ServerConfig.hpp"
#include "ConfigValidator.hpp"
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

class	Config {
	private:
		std::vector<std::string>	tokens;
		ConfigValidator				validator;
		std::vector<ServerConfig>	Servers;

		void	tokenize(const std::string& filePath);
		void	parseConfig();
		ServerConfig	parseServerBlock(size_t& start_index);
		void	decideDefaultServer(std::vector<ServerConfig> &Servers);

	public:
		void	loadConfigFile(const std::string &filename);
		const std::vector<ServerConfig>&	getServerConfigs() const;
};

#endif
