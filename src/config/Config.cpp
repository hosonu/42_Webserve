#include "Config.hpp"

void	Config::tokenize(const std::string& filePath) {
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
		//std::cout << token << std::endl;
		#endif

		if (token[token.length() - 1] == ';') {
			std::string word = token.substr(0, token.size() - 1);
			this->tokens.push_back(word);
			this->tokens.push_back(";");
		} else {
			this->tokens.push_back(token);
		}
	}
}

void	Config::parseConfig() {
	for (size_t i = 0; i < this->tokens.size(); ++i) {
		if (this->tokens[i] == "server") {
			ServerConfig server_config = parseServerBlock(i);
			this->Servers.push_back(server_config);
		}
	}
}

ServerConfig	Config::parseServerBlock(size_t& start_index) {
	ServerConfig	server_config;
	
	for (size_t i = start_index + 1; i < tokens.size(); ++i) {
		if (tokens[i] == "listen") {
			server_config.setListen(tokens[i + 1]);
		} else if (tokens[i] == "server_name") {
			server_config.setServerName(tokens[i + 1]);
		} else if (tokens[i] == "error_page") {
			server_config.setErrorPage(tokens, i);
		} else if (tokens[i] == "client_max_body_size") {
			server_config.setClientMaxBodySize(tokens[i + 1]);
		} else if (tokens[i] == "location") {
			server_config.addLocation(this->tokens, i);
		} else if (tokens[i] == "}") {
			break;
		}
	}
	server_config.sortLocations();
	return server_config;
}


void	Config::decideDefaultServer(std::vector<ServerConfig> &Servers) {
	int default_port = Servers[0].getListenPort();
	std::string default_ip = Servers[0].getListenHost();
	Servers[0].setDefault(true);
	for (size_t i = 1; i < Servers.size(); ++i) {
		if(Servers[i].getListenPort() != default_port || Servers[i].getListenHost() != default_ip) {
			Servers[i].setDefault(true);
		} else {
			Servers[i].setDefault(false);
		}
	}
}

void	Config::loadConfigFile(const std::string& filename) {
	tokenize(filename);
	this->validator.validate(this->tokens);
	parseConfig();
	decideDefaultServer(this->Servers);
}

const std::vector<ServerConfig>& Config::getServerConfigs() const {
	return this->Servers;
}
