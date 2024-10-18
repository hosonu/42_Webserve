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
		} if (line.find(".") != std::string::npos) {
			currentServer.host = line.substr(start, end - start);			
		} else {
			std::stringstream ss(line.substr(start, end - start));
			int port;
			ss >> port;
			currentServer.listenPort = port;
		}
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
	int i = 0;
	while(std::getline(streamConf, line)) {
		if (line.find("server {") != std::string::npos) {
			isServerBlock = true;
			currentServer = ServerConfig();
		} else if (line.find("}") != std::string::npos) {
			if (isServerBlock) {
				this->Servers.push_back(currentServer);
				isServerBlock = false;
				i = 0;
			}
		} else if (isServerBlock) {
			/* Parse individual directives inside server block*/
			if (line.find("listen") != std::string::npos) {
				getListenDirective(line, currentServer);
			} else if (line.find("server_name") != std::string::npos) {
				size_t start = line.find("server_name ") + 11;
				size_t end = line.find(";", start);
				std::string serverName = line.substr(start, end - start);
				currentServer.serverName = serverName;
			} else if (line.find("error_page") != std::string::npos) {
				size_t start = line.find("error_page ") + 11;
				size_t end = line.find(";", start);
				std::string errorPage = line.substr(start, end - start);
				currentServer.errorPages.insert({i, errorPage});// need to fix for std::map errorPages
				i++;
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