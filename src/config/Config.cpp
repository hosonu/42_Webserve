#include "Config.hpp"

Config::Config() : Servers(10, ServerConfig()) {
}

Config::~Config() {
}

bool	Config::parse(const std::string &filePath) {
	std::ifstream file(filePath.c_str());
	if (!file.is_open()) {
		std::cerr << "Failed to open configuration file: " << filePath << std::endl;
		return false;
	}

	std::stringstream streamConf;
	streamConf << file.rdbuf();//return buf to contain the entire file & add it to buffer
	
	//Parse the file stream
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
		} else if (isServerBlock) {
			// Parse individual directives inside server block
			if (line.find("listen") != std::string::npos) {
				std::stringstream ss(line.substr(line.find("listen ") + 6, line.find(";")));
				int port;
				ss >> port;
				std::cout << "here listen port: " << port << std::endl;//debug
				currentServer.setListenPort(port);
			} else if (line.find("host") != std::string::npos) {
				std::string host = line.substr(line.find("host ") + 5, line.find(";"));
				std::cout << "here host: " << host << std::endl;//debug
				currentServer.setHost(host);
			}
		}
	}

	file.close();
	return true;
}