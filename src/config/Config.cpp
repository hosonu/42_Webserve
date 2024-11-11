#include "Config.hpp"

Config::Config() {
}

Config::~Config() {
}

// void	initializeRouteData(ServerConfig &currentServer) {
// 	currentServer.routeData.path = "/";
// 	currentServer.routeData.root = "";
// 	currentServer.routeData.autoindex = false;
// 	currentServer.routeData.indexFile = "index.html";
// }

//if there are some words in sever {}, it detect error 
bool checkFileStruct(std::stringstream &file) {

	std::string stringFile = file.str();
	int braceCount = 0;

/*should checks and fixed*/
	for (size_t i = 0; i < stringFile.length(); ++i){
		if (stringFile[i] == '{' || stringFile[i] == '}' || stringFile[i] == ';') {
			if (stringFile[i] == '{') {
				braceCount++;
			} else if (stringFile[i] == '}') {
				braceCount--;
			}
			if (stringFile[i + 2] != '\n') {
				std::cerr << "Syntax error: " << stringFile[i + 2] << ": no line breaks" << std::endl;
				return false;
			}
		} else if (stringFile[i] == '\t') {
			if (!std::isalpha(stringFile[i + 1]) && stringFile[i + 1] != '}' && stringFile[i] != '\t') {
				std::cerr << "Syntax error: " << stringFile[i + 1] << ": no tokens" << std::endl;
				return false;
			}
		} else if (stringFile[i] == ' ') {			
			if (!std::isalnum(stringFile[i + 1]) && stringFile[i + 1] != '/' && stringFile[i + 1] != '{') {
				std::cerr << "Syntax error: " << stringFile[i + 1] << ": no appropriate parameter" << std::endl;
				return false;
			}
		}
	}
	if (braceCount != 0) {
		std::cerr << "Syntax error: Mismatched braces" << std::endl;
		return false;
	}

	if (checkValidDirective(file) != true)
		return false;
	

	return true;
}

/*parametars check*/
bool checkServerConfigs(const std::vector<ServerConfig>& servers) {
    for (std::vector<ServerConfig>::const_iterator server = servers.begin(); server != servers.end(); ++server) {
        if (server->listenPort < 0 || server->listenPort > 65535) {
            std::cerr << "Invalid port number : " << server->listenPort << std::endl;
            return false;
        }
        if (server->host.empty() || !isValidIpAddress(server->host)) {
            std::cerr << "Invalid Ip Address : " << server->host << std::endl;
            return false;
        }
        if (!isValidErrorPages(server->errorPages)) {
            std::cerr << "Invalid Error pages: ";
            for (std::map<int, std::string>::const_iterator it = server->errorPages.begin(); 
                 it != server->errorPages.end(); ++it) {
					if (it->first > 599 || it->first < 400)
                		std::cerr << it->first << ": " << it->second << ".";
            }
            std::cerr << std::endl;
            return false;
        }
        if (server->maxBodySize.empty() || !isValidMaxBodySize(server->maxBodySize)) {
            std::cerr << "Invalid MaxBodySize : " << server->maxBodySize << std::endl;
            return false;
        }
        if (!isValidRouteData(server->LocationData)) {
            return false;
        }
    }
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
				size_t start = line.find("client_max_body_size ") + 21;
				size_t end = line.find(";", start);
				std::string clientMaxBodySize = line.substr(start, end - start);
				currentServer.maxBodySize = clientMaxBodySize;
			} else if (line.find("location") != std::string::npos) {
				currentServer.LocationData.push_back(getRouteData(line, streamConf));
			}
		}
	}


	/*add func to checks invalid parameter*/
	if (checkServerConfigs(this->Servers) != true) {
		return false;
	}

	file.close();
	return true;
}

std::vector<ServerConfig> Config::getServerConfig() const {
	return Servers;
}
