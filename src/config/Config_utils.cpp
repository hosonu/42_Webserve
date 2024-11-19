#include "Config_utils.hpp"

void	getListenDirective(std::string &line, ServerConfig &currentServer) {
	size_t start = line.find("listen ") + 7;
	size_t end = line.find(";", start);
	if (line.find(":") != std::string::npos) {
		int posColon = line.find(":", start);
		currentServer.host = line.substr(start, posColon - start);
		std::stringstream ss(line.substr(posColon + 1, end - (posColon + 1)));
		int port;
		if (!(ss >> port) || !ss.eof()) {
			currentServer.listenPort = -1;
			return ;
		}
		currentServer.listenPort = port;
	}
}

static std::vector<std::string> splitString(std::string &input) {
	std::vector<std::string> result;
	std::istringstream iss(input);
	std::string token;

	while (iss >> token) {
		if (!token.empty() && token[token.length() - 1] == ';') {
			token = token.substr(0, token.length() - 1);
		}
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
	std::string line_2;	
	while (std::getline(streamConf, line_2)) {
		if (line_2.find("root") != std::string::npos) {
			size_t start = line_2.find("root ") + 5;
			size_t end = line_2.find(";", start);
			std::string rootPath = line_2.substr(start, end - start);
			locationDatum.root = rootPath;
		} else if (line_2.find("allow_methods") != std::string::npos) {
			line_2 = line_2.substr(line_2.find("allow_methods") + 14);
			std::vector<std::string> tokens = splitString(line_2);
			locationDatum.allowMethods = tokens;
		} else if (line_2.find("autoindex") != std::string::npos) {
			size_t start = line_2.find("autoindex ") + 10;
			size_t end = line_2.find(";", start);
			std::string autoindex = line_2.substr(start, end - start);
			//std::cout << "autoindex: " << autoindex << std::endl;
			if (autoindex == "on")
				locationDatum.autoindex = true;
			else if ((autoindex == "off"))
				locationDatum.autoindex = false;
			else
				throw std::logic_error("[emerg] unexpected token in autoindex line_2 in ");
		} else if (line_2.find("index") != std::string::npos) {
			locationDatum.indexFile = "index.html";
		}
		if (line_2.find("}") != std::string::npos)
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
			throw std::invalid_argument("[emerg] unexpected path in location{} in ");
		}

		for(size_t i = 0; i < it->allowMethods.size(); ++i) {
			std::string method = it->allowMethods[i];
			if (method != "GET" && method != "POST" && method != "DELETE") {
				throw std::invalid_argument("[emerg] unexpected methods in location{} in ");
			}
		}

		if (it->indexFile.length() < 5 || 
		it->indexFile.substr(it->indexFile.length() - 5) != ".html") {
			throw std::invalid_argument("[emerg] unexpected indexFile in location{} in ");
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
