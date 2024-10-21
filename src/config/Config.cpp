#include "Config.hpp"

Config::Config() {
}

Config::~Config() {
}

void	getListenDirective(std::string &line, ServerConfig &currentServer) {
	size_t start = line.find("listen ") + 7;
	size_t end = line.find(";", start);
	if (line.find(":") != std::string::npos) {
		int posColon = line.find(":", start);
		if (line.find("localhost") != std::string::npos) {
			currentServer.host = "127.0.0.1";
		} else {
			currentServer.host = line.substr(start, posColon - start);
		}
		std::stringstream ss(line.substr(posColon + 1, end - posColon));
		int port;
		ss >> port;
		currentServer.listenPort = port;
	} else {
		if (line.find("localhost") != std::string::npos) {
			currentServer.host = "127.0.0.1";
			currentServer.listenPort = 80;
		} if (line.find(".") != std::string::npos) {
			currentServer.host = line.substr(start, end - start);
			currentServer.listenPort = 80;
		} else {
			std::stringstream ss(line.substr(start, end - start));
			int port;
			ss >> port;
			currentServer.listenPort = port;
			currentServer.host = "127.0.0.1";//default host
		}
	}
}

std::vector<std::string> splitString(std::string &input) {
	std::vector<std::string> result;
	std::istringstream iss(input);
	std::string token = " ";

	while (iss >> token) {
		result.push_back(token);
	}

	return result;
}


void	getErrorPage(std::string &line, ServerConfig &currentServer) {
	std::vector<std::string> tokens = splitString(line);
	// for (size_t i = 0; i < tokens.size(); ++i) {
	// 	std::cout << tokens[i] << std::endl;
	// }

	std::string path = tokens[tokens.size() - 1].substr(0, tokens[tokens.size() - 1].find(";"));
	// std::cout << "path: " << path << std::endl;
	for (size_t i = 1; i < tokens.size() - 1; ++i) {
			int statusCode = std::strtol(tokens[i].c_str(), 0, 10);
			currentServer.errorPages[statusCode] = path;
	}
}

bool	Config::parse(const std::string &filePath) {
	std::ifstream file(filePath.c_str());
	if (!file.is_open()) {
		std::cerr << "Failed to open configuration file: " << filePath << std::endl;
		return false;
	}

	std::stringstream streamConf;
	streamConf << file.rdbuf();//return buf to contain the entire file & add it to buffer
	
	/*TODO: check error of file contens*/
	// std::string fileContens.
	// if (checFileStruct())

	/*Parse the file stream*/
	std::string line;
	ServerConfig currentServer;
	bool isServerBlock = false;
	while(std::getline(streamConf, line)) {
		if (line.find("server {") != std::string::npos) {
			isServerBlock = true;
			currentServer = ServerConfig();
		} else if (line.find("}") != std::string::npos) {
			if (isServerBlock) {
				this->Servers.push_back(currentServer);
				isServerBlock = false;
			}
			/* Parse individual directives inside server block*/
		} else if (isServerBlock) {
			if (line.find("listen") != std::string::npos) {
				getListenDirective(line, currentServer);
			} else if (line.find("server_name") != std::string::npos) {
				size_t start = line.find("server_name ") + 11;
				size_t end = line.find(";", start);
				std::string serverName = line.substr(start, end - start);
				currentServer.serverName = serverName;
			} else if (line.find("error_page") != std::string::npos) {
				getErrorPage(line, currentServer);
			} else if (line.find("client_max_body_size") != std::string::npos) {
				size_t start = line.find("client_max_body_size ") + 20;
				size_t end = line.find(";", start);
				std::string clientMaxBodySize = line.substr(start, end - start);
				currentServer.maxBodySize = clientMaxBodySize;
			}
		}
	}

	file.close();
	return true;
}




std::vector<ServerConfig> Config::getServerConfig() const {
	return Servers;
}