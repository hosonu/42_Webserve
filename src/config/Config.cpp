#include "Config.hpp"

Config::Config() {
}

Config::~Config() {
}

void	initializeRouteData(ServerConfig &currentServer) {
	currentServer.routeData.path = "";
	currentServer.routeData.root = "";
	currentServer.routeData.autoindex = false;
	currentServer.routeData.indexFile = "";
}

//if there are some words in sever {}, it detect error 
bool checkFileStruct(std::stringstream &file) {

	/*check sysntax error ~start~*/
	std::string stringFile = file.str();
	for (size_t i = 0; i < stringFile.size(); ++i){
		if (stringFile[i] == '{' || stringFile[i] == '}' || stringFile[i] == ';') {
			if (stringFile[i + 2] != '\n') {
				std::cerr << "syntax error: " << stringFile[i + 2] << ": no line breaks" << std::endl;
				return false;
			}
		} else if (stringFile[i] == '\t') {
			if (!std::isalpha(stringFile[i + 1]) && stringFile[i + 1] != '}' && stringFile[i] != '\t') {
				std::cerr << "syntax error: " << stringFile[i + 1] << ": no tokens" << std::endl;
				return false;
			}
		} else if (stringFile[i] == ' ') {			
			if (!std::isalnum(stringFile[i + 1]) && stringFile[i + 1] != '/' && stringFile[i + 1] != '{') {
				std::cerr << "syntax error: " << stringFile[i + 1] << ": no appropriate parameter" << std::endl;
				return false;
			}
		}
	}

	if (checkBraceFlags(file) != true)
		return false;
	

	return true;
}

/*parametars check*/
bool checkServerConfig(const ServerConfig& currentServer) {
	if (currentServer.listenPort < 0 || currentServer.listenPort > 65535) {
		std::cerr << "Invalid port number : " << currentServer.listenPort << std::endl;
		return false;
	}
	if (currentServer.host.empty() || !isValidIpAddress(currentServer.host)) {
		std::cerr << "Invalid Ip Address : " << currentServer.host << std::endl;
		return false;
	}
	if (!isValidErrorPages(currentServer.errorPages)) {
		std::cerr << "Invalid ErrorPages : " << std::endl;
		return false;
	}
	if (currentServer.maxBodySize.empty() || !isValidMaxBodySize(currentServer.maxBodySize)) {
		std::cerr << "Invalid MaxBodySize : " << currentServer.maxBodySize << std::endl;
		return false;
	}
	// if (currentServer.routeData.empty()) {
	// 	return false;
	// }
	return true;
}

bool	Config::parse(const std::string &filePath) {
	std::ifstream file(filePath.c_str());
	if (!file.is_open()) {
		std::cerr << "Failed to open configuration file: " << filePath << std::endl;
		return false;
	}

	
	std::stringstream streamConf;
	streamConf << file.rdbuf();//return buf to contain the entire file & add it to buffer

	if (checkFileStruct(streamConf) == false) {
		return false;
	}
	/*TODO: check error of file contens*/

	/*Parse the file stream*/
	std::string		line;
	ServerConfig	currentServer;
	bool			isServerBlock = false;
	bool			isLocationBlock = false;
	while(std::getline(streamConf, line)) {
		if (line.find("server {") != std::string::npos) {
			isServerBlock = true;
			currentServer = ServerConfig();
		} else if (line.find("}") != std::string::npos) {
			if (isServerBlock && isLocationBlock != true) {
				this->Servers.push_back(currentServer);
				isServerBlock = false;
			}
			if (isLocationBlock) {
				isLocationBlock = false;
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
				size_t start = line.find("client_max_body_size ") + 21;
				size_t end = line.find(";", start);
				std::string clientMaxBodySize = line.substr(start, end - start);
				currentServer.maxBodySize = clientMaxBodySize;
			} else if (line.find("location") != std::string::npos) {
				initializeRouteData(currentServer);
				isLocationBlock = true;
			} else if (isLocationBlock == true) {
				getRouteData(line, currentServer);
			}
		}
	}


	/*add func to checks invalid parameter*/
	if (checkServerConfig(currentServer) != true) {
		return false;
	}

	file.close();
	return true;
}

std::vector<ServerConfig> Config::getServerConfig() const {
	return Servers;
}
