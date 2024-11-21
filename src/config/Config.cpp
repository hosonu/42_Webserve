#include "Config.hpp"
#include <algorithm>

Config::Config() {
}

Config::~Config() {
}

bool Config::checkFileStruct(std::stringstream &file, const std::string &filePath) {

	std::string stringFile = file.str();
	int braceCount = 0;
	for (size_t i = 0; i < stringFile.length(); ++i){
		int lineNumber = std::count(stringFile.begin(), stringFile.begin() + i, '\n') + 1; // Calculate line number
		if (stringFile[i] == '{' || stringFile[i] == '}' || stringFile[i] == ';') {
			if (stringFile[i] == '{') {
				braceCount++;
			} else if (stringFile[i] == '}') {
				braceCount--;
			}
			if (stringFile[i + 1] != '\n') {
				throw std::logic_error("[emerg] unexpected end of line, expecting \"\\n\" in " + filePath + ": " + customToString(lineNumber));
			}
		} else if (stringFile[i] == '\t') {
			if (!std::isalpha(stringFile[i + 1]) && stringFile[i + 1] != '}' && stringFile[i] != '\t') {
				throw std::logic_error("[emerg] no directive in " + filePath + ": " + customToString(lineNumber));
			}
		} else if (stringFile[i] == ' ') {			
			if (!std::isalnum(stringFile[i + 1]) && stringFile[i + 1] != '/' && stringFile[i + 1] != '{') {
				throw std::logic_error("[emerg] unexpected tokens in " + filePath + ": " + customToString(lineNumber));
			}
		}
		if (i == stringFile.length() - 1) {
			if (braceCount != 0) {
				throw std::logic_error("[emerg] unexcepted \"}\" in " + filePath + ": " + customToString(lineNumber));
			}
		}
	}
	checkValidDirective(file, filePath);
	return true;
}

/*parametars check*/
void	Config::checkServerConfigs(const std::vector<ServerConfig>& servers, const std::string &filePath) {
    for (std::vector<ServerConfig>::const_iterator server = servers.begin(); server != servers.end(); ++server) {
        if (server->listenPort < 0 || server->listenPort > 65535) {
			throw std::logic_error("[emerg] unexepcted paramaeter in \"port\" directive in " + filePath);
        }
        if (server->host.empty() || !isValidIpAddress(server->host)) {
			throw std::logic_error("[emerg] unexepcted paramaeter in \"listen\" directive in " + filePath);
        }
        if (!isValidErrorPages(server->errorPages)) {
            std::cerr << "Invalid Error pages: ";
            for (std::map<int, std::string>::const_iterator it = server->errorPages.begin(); 
                 it != server->errorPages.end(); ++it) {
					if (it->first > 599 || it->first < 400)
                		std::cerr << it->first << ": " << it->second << ".";
            }
			throw std::logic_error("[emerg] unexepcted paramaeter in \"listen\" directive in " + filePath);
        }
        if (server->maxBodySize.empty() || !isValidMaxBodySize(server->maxBodySize)) {
			throw std::logic_error("[emerg] unexepcted paramaeter in \"client_max_body_size\" directive in " + filePath);
        }
        if (!isValidRouteData(server->LocationData)) {
            throw std::logic_error("[emerg] unexepcted paramaeter in \"location\" directive in " + filePath);
        }
    }
}

bool	Config::parse(const std::string &filePath) {
	std::ifstream file(filePath.c_str());
	if (!file.is_open()) {
		throw std::invalid_argument("[emerg] open() " + filePath + " failed (No such file or directory)");
	}
	if (file.peek() == std::ifstream::traits_type::eof()) {
		throw std::invalid_argument("[emerg] " + filePath + " is empty");
	}

	std::stringstream streamConf;
	streamConf << file.rdbuf();
	
	
	
	if (checkFileStruct(streamConf, filePath) == false) {
		return false;
	}



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
	checkServerConfigs(this->Servers, filePath);
	decideDefaultServer(this->Servers);
	file.close();
	return true;
}

std::vector<ServerConfig> Config::getServerConfig() const {
	return Servers;
}
