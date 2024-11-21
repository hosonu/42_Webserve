#ifndef HCONFIG_HPP
#define HCONFIG_HPP

#include "HServerConfig.hpp"
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

class	HConfig {
	private:
		std::vector<std::string>	tokens;
		ConfigValidator				validator;
		std::vector<HServerConfig>	Servers;

		void	tokenize(const std::string& filePath);
		void	parseConfig();
		HServerConfig	parseServerBlock(size_t& start_index);

	public:
		void	loadConfigFile(const std::string &filename);
		const std::vector<HServerConfig>&	getServerConfigs() const;
};

#endif
