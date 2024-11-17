#include "Config_utils.hpp"

/*if we have to set as follows "host:port", this func should be changed*/
void	getListenDirective(std::string &line, ServerConfig &currentServer) {
	size_t start = line.find("listen ") + 7;
	size_t end = line.find(";", start);
	if (line.find(":") != std::string::npos) {
		int posColon = line.find(":", start);
		//if (line.find("localhost") != std::string::npos) {
		//	currentServer.host = "127.0.0.1";
		//} else {
		//}
		currentServer.host = line.substr(start, posColon - start);
		std::stringstream ss(line.substr(posColon + 1, end - (posColon + 1)));
		int port;
		if (!(ss >> port) || !ss.eof()) {
			currentServer.listenPort = -1;
			return ;
		}
		currentServer.listenPort = port;
	}
	// else {
	// 	if (line.find("localhost") != std::string::npos) {
	// 		currentServer.host = "127.0.0.1";
	// 		currentServer.listenPort = 80;
	// 	} if (line.find(".") != std::string::npos) {
	// 		currentServer.host = line.substr(start, end - start);
	// 		currentServer.listenPort = 80;
	// 	} else {
	// 		std::stringstream ss(line.substr(start, end - start));
	// 		int port;
	// 		if (!(ss >> port) || !ss.eof()) {
	// 		currentServer.listenPort = -1;
	// 		} else {
	// 			currentServer.listenPort = port;
	// 		}
	// 		currentServer.host = "0.0.0.0";//default host
	// 	}
	// }
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
		bool isValidNum = false;
		for (size_t j = 0; j < tokens[i].length(); ++j) {
			/*ありえない値が来た時の対処、intを超えるなど*/
			if (!std::isdigit(tokens[i][j]))
				isValidNum = true;
		}
		if (isValidNum == true) {
			currentServer.errorPages[-1] = path;
		} else {
			int statusCode = std::strtol(tokens[i].c_str(), 0, 10);
			if (currentServer.errorPages[statusCode].empty() == false)
				throw std::logic_error("[emerg] unexepcted paramaeter in \"listen\" directive");
			currentServer.errorPages[statusCode] = path;
		}
	}
}

 void	initializeRouteData(Route &routeData) {
 	routeData.path = "/";
 	routeData.root = "";
 	routeData.autoindex = false;
 	routeData.indexFile = "index.html";
 }

Route	getRouteData(std::string &line, std::stringstream &streamConf) {
	Route locationDatum;

	initializeRouteData(locationDatum);
	size_t start = line.find("location ") + 9;
	size_t end = line.find("{", start);
	std::string path = line.substr(start, end - start);
	locationDatum.path = path;
	while (std::getline(streamConf, line)) {
		if (line.find("root") != std::string::npos) {
			size_t start = line.find("root ") + 5;
			size_t end = line.find(";", start);
			std::string rootPath = line.substr(start, end - start);
			locationDatum.root = rootPath;
		} else if (line.find("allow_methods") != std::string::npos) {
			std::vector<std::string> tokens = splitString(line);
			locationDatum.allowMethods = tokens;
		} else if (line.	find("autoindex") != std::string::npos) {
			size_t start = line.find("autoindex ") + 10;
			size_t end = line.find(";", start);
			std::string autoindex = line.substr(start, end - start);

			/*if on or off not here should return*/
			if (autoindex == "on")
				locationDatum.autoindex = true;
			else
				locationDatum.autoindex = false;
		} else if (line.find("index") != std::string::npos) {
			locationDatum.indexFile = "index.html";
		}
		if (line.find("}"))
			break;
	}
	return locationDatum;
}

/*---------------------------------------checks syntax error-------------------------------------------*/
bool	checksDirectiveExist(const std::string &line) {
	return line == "server" ||
			line == "listen" ||
			line == "server_name" ||
			line == "error_page" ||
			line == "client_max_body_size" ||
			line == "location" ||
			line == "allow_methods" ||
			line == "root" ||
			line == "index" ||
			line == "location" ||
			line == "autoindex" ||
			line == "root";
}

void	checkValidDirective(std::stringstream &file, const std::string &filePath) {
	std::string line;
	std::stringstream tmp;
    tmp << file.rdbuf();
    file.clear();
    file.seekg(0);
	
	size_t line_cnt = 1;
	while(std::getline(tmp, line)) {
		if (line.length() > 1) {
			int start = line.find_first_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ}");
			int end = line.find_first_of(" \t}", start);
			std::string word = line.substr(start, end - start);
			#ifdef DEBUG
			std::cout << "start=[" << start << "] end=[" << end << "] back=[" << (int)line[line.length() - 1] << "]" << line << std::endl;
			#endif
			if (end > 1 && line.find("}") == std::string::npos && checksDirectiveExist(word) == false) {
				throw std::logic_error("[emerg] invalid directive in " + filePath + ": " + customToString(line_cnt));
			}
			if (line[line.length() - 1] != '{' && line[line.length() - 1] != '}') {
				if (line[line.length() - 1] != ';') {
					throw std::logic_error("[emerg] unexcepted \";\" end of line in " + filePath + ": " + customToString(line_cnt));
				}
			}
			if (end < 0) {
				throw std::logic_error("[emerg] unexcepted syntax in " + filePath + ": " + customToString(line_cnt));
			}
		} else if (line.length() == 1 && line != "}") {
			throw std::logic_error("[emerg] unexcepted syntax in " + filePath + ": " + customToString(line_cnt));
		}
		line_cnt++;
	}
}

/*-----------------------------------------validate Parmeters-----------------------------------------*/
bool isValidOctet(const std::string &octet) {
    if (octet.empty() || octet.length() > 3) {
        return false;
    }
    int value = 0;
    for (size_t j = 0; j < octet.length(); ++j) {
        if (!isdigit(octet[j])) {
            return false;
        }
        value = value * 10 + (octet[j] - '0');
    }
    if (value > 255) {
        return false;
    }
    if (octet.length() > 1 && octet[0] == '0') {
        return false;
    }
    return true;
}

bool	isValidIpAddress(const std::string &ip) {

	if (ip == "localhost") {
		return true;
	}
	int dots = 0;
	for (size_t i = 0; i < ip.length(); ++i) {
		if (ip[i] == '.')
			dots++;
	}
	if (dots != 3) {
		return false;
	}

	std::string octet;
	for (size_t i = 0; i < ip.length(); ++i) {
		if (ip[i] == '.') {
			if (!isValidOctet(octet)) {
                return false;
            }
            octet.clear();
		} else {
			octet += ip[i];
		}
	}

	if (!isValidOctet(octet)) {
		return false;
	}

	return true;
}

bool isValidErrorPages(const std::map<int, std::string>& errorPages) {
	std::map<int, std::string>::const_iterator it;
	for (it = errorPages.begin(); it != errorPages.end(); ++it) {
		//errorPageの範囲を確認
		if (it->first < 400 || it->first > 599) {
			return false;
		}
		if (it->second.empty()) {
			return false;
		}
		if (it->second.length() < 5 || 
			it->second.substr(it->second.length() - 5) != ".html") {
			return false;
		}
	}
	return true;
}

bool	isValidMaxBodySize(const std::string &maxBodySize) {
	if (maxBodySize.length() < 2) {
		return false;
	}
	if (maxBodySize[maxBodySize.length() - 1] != 'm') {
		return false;
	}
	std::string num = maxBodySize.substr(0, maxBodySize.length() - 1);
	for (size_t i = 0; i < num.size(); ++i) {
		if (!std::isdigit(num[i])) {
			return false;
		}
		if (num[0] == 0 && i > 1) {
			return false;
		}
	}
	return true;
}

bool	isValidRouteData(const std::vector<Route> &locationData) {

	for (std::vector<Route>::const_iterator it = locationData.begin(); it < locationData.end(); ++it) {
		if (it->path[0] != '/') {
			std::cerr << "Invalid path: "<< it->path << ": Path must start with '/'" << std::endl;
			return false;
		}

		for(size_t i = 0; i < it->allowMethods.size(); ++i) {
			std::string method = it->allowMethods[i];
			if (method != "GET" && method != "POST" && method != "DELETE") {
				std::cerr << "Invalid method: Only GET, POST, and DELETE are allowed" << std::endl;
				return false;
			}
		}

		if (it->indexFile.length() < 5 || 
		it->indexFile.substr(it->indexFile.length() - 5) != ".html") {
			std::cerr << "Invalid index file: Index file must end with '.html'" << std::endl;
			return false;
		}
	}

	return true;

}

void	decideDefaultServer(std::vector<ServerConfig> &Servers) {
	int default_port = Servers.front().listenPort;
	std::string default_ip = Servers.front().host;
	Servers[0].is_default = true;
	for (size_t i = 1; i < Servers.size(); ++i) {
		if(Servers[i].listenPort != default_port || Servers[i].host != default_ip) {
			Servers[i].is_default = true;
		} else {
			Servers[i].is_default = false;
		}
	}
}
