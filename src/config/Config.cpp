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
			currentServer.host = "0.0.0.0";//default host
		}
	}
}

static std::vector<std::string> splitString(std::string &input) {
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
	std::string path = tokens[tokens.size() - 1].substr(0, tokens[tokens.size() - 1].find(";"));
	for (size_t i = 1; i < tokens.size() - 1; ++i) {
			int statusCode = std::strtol(tokens[i].c_str(), 0, 10);
			currentServer.errorPages[statusCode] = path;
	}
}

void	initializeRouteData(ServerConfig &currentServer) {
	currentServer.routeData.path = "";
	currentServer.routeData.root = "";
	currentServer.routeData.autoindex = false;
	currentServer.routeData.indexFile = "";
}

void	getRouteData(std::string &line, ServerConfig &currentServer) {
	if (line.find("root") != std::string::npos) {
		size_t start = line.find("root ") + 5;
		size_t end = line.find(";", start);
		std::string rootPath = line.substr(start, end - start);
		currentServer.routeData.root = rootPath;
	} else if (line.find("allow_methods") != std::string::npos) {
		std::vector<std::string> tokens = splitString(line);
		currentServer.routeData.allowMethods = tokens;
	} else if (line.find("autoindex") != std::string::npos) {
		size_t start = line.find("autoindex ") + 10;
		size_t end = line.find(";", start);
		std::string autoindex = line.substr(start, end - start);
		if (autoindex == "on")
			currentServer.routeData.autoindex = true;
		else
			currentServer.routeData.autoindex = false;
	} else if (line.find("index") != std::string::npos) {
		currentServer.routeData.indexFile = "index.html";
	}
}


bool checkFileStruct(std::stringstream &file) {

	/*check sysntax error ~start~*/
	std::string stringFile = file.str();
	for (size_t i = 0; i < stringFile.size(); ++i){
		std::cout << stringFile[i] << std::flush;
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
	/*check sysntax error ~end~*/

	std::string line;
	int	cnt_line = 1;
	int braceCount = 0;
	bool serverFlag = false;
	bool errorPageFlag = false;
	bool clientMaxBodySizeFlag = false;
	bool locationFlag = false;
	while(std::getline(file, line)) {
		for (size_t i = 0; i < line.length(); ++i) {
			if (line[i] == '{') braceCount++;
			if (line[i] == '}') braceCount--;
		}
		if (cnt_line == 1 && line.find("server") != std::string::npos)
			serverFlag = true;
		if (cnt_line > 1 && braceCount != 0 && line.find("error_pager") != std::string::npos)
			errorPageFlag = true;
		if (cnt_line > 1 && braceCount != 0 && line.find("client_max_body_size") != std::string::npos)
			clientMaxBodySizeFlag = true;
		if (cnt_line > 1 && braceCount != 0 && line.find("location") != std::string::npos)
			locationFlag = true;

		if (braceCount == 0 && line.length() == 1) {
			serverFlag = false;
			errorPageFlag = false;
			clientMaxBodySizeFlag = false;
			locationFlag = false;
			cnt_line = 0;
		}
		cnt_line++;
 	}
	if (braceCount != 0 && serverFlag == false && errorPageFlag == false && 
	clientMaxBodySizeFlag == false && locationFlag == false) {
		std::cerr << "sysntax error: Not enough TOKEN required." << std::endl;
		return false;
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

	/*TODO: check error of file contens*/
	if (checkFileStruct(streamConf) == false)
		return false;

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
				size_t start = line.find("client_max_body_size ") + 20;
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

	file.close();
	return true;
}

std::vector<ServerConfig> Config::getServerConfig() const {
	return Servers;
}
