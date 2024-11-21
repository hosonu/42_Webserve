#include "HConfig.hpp"

void	HConfig::tokenize(const std::string& filePath) {
	std::ifstream file(filePath.c_str());
	if (!file.is_open()) {
		throw std::invalid_argument("[emerg] open() " + filePath + " failed (No such file or directory)");
	}
	if (file.peek() == std::ifstream::traits_type::eof()) {
		throw std::invalid_argument("[emerg] " + filePath + " is empty");
	}
	std::stringstream streamConf;
	streamConf << file.rdbuf();

	std::string	token;
	while (streamConf >> token) {
		#ifdef DEBUG
		std::cout << token << std::endl;
		#endif

		if (token.back() == ';') {
			std::string word = token.substr(0, token.size() - 1);
			this->tokens.push_back(word);
			this->tokens.push_back(";");
		} else {
			this->tokens.push_back(token);
		}
	}
}

void	HConfig::parseConfig() {
	for (size_t i = 0; i < this->tokens.size(); ++i) {
		if (this->tokens[i] == "server") {
			HServerConfig server_config = parseServerBlock(i);
			this->Servers.push_back(server_config);
		}
	}
}

HServerConfig	parseServerBlock(size_t& start_index) {
	HServerConfig	server_config;
	
	for (size_t i = start_index + 1; i < this->tokens.size(); ++i) {
		if (tokens[i] == "listen") {
			server_config.setListen(tokens[i + 1]);
		} else if (tokens[i] == "server_name") {
			server_config.setServerName(tokens[i + 1]);
		} else if (tokens[i] == "error_page") {
			server_config.setErrorPage(tokens[i + 1]);
		} else if (tokens[i] == "client_max_body_size") {
			server_config.setClientMaxBodySize(tokens[i + 1]);
		} else if (tokens[i] == "location") {
			server_config.addLocation(this->tokens, i);
		} else if (tokens[i] == "}") {
			break;
		}
	}
	return server_config;
}

void	HConfig::loadConfigFile(const std::string& filename) {
	tokenize(filename);
	this->validator.validate(this->tokens);
	parseConfig();
}

const std::vector<ServerConfig>& HConfig::getServerConfigs() const {
	return this->Servers;
}
