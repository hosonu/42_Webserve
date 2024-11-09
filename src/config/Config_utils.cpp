#include "Config_utils.hpp"

/*if we have to set as follows "host:port", this func should be changed*/
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
				throw std::runtime_error("double booking");
			currentServer.errorPages[statusCode] = path;
		}
	}
}

void	getRouteData(std::string &line, ServerConfig &currentServer) {
	if (line.find("location") != std::string::npos) {
		size_t start = line.find("location ") + 9;
		size_t end = line.find("{", start);
		std::string path = line.substr(start, end - start);
		currentServer.routeData.path = path;
	} else if (line.find("root") != std::string::npos) {
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

		/*if on or off not here should return*/
		if (autoindex == "on")
			currentServer.routeData.autoindex = true;
		else
			currentServer.routeData.autoindex = false;
	} else if (line.find("index") != std::string::npos) {
		currentServer.routeData.indexFile = "index.html";
	}
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

bool checkValidDirective(std::stringstream &file) {
	std::string line;
	std::stringstream tmp;
    tmp << file.rdbuf();
    file.clear();
    file.seekg(0);
	
	size_t line_cnt = 0;
	while(std::getline(tmp, line)) {
		if (!line.empty() && line.back() == '\r') {
			line.pop_back();
		}
		if (line.length() > 1) {
			int start = line.find_first_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ}");
			int end = line.find_first_of(" \t}", start);
			std::string word = line.substr(start, end - start);
			if (end > 1 && line.find("}") == std::string::npos && checksDirectiveExist(word) == false) {
				std::cerr << "Syntax error: " << word << " is not valid tokens" << std::endl;
				return false;
			}
			// std::cout << "start=[" << start << "] end=[" << end << "] back=[" << (int)line.back() << "]" << line << std::endl;
			if (line.back() != '{' && line.back() != '}') {
				if (line.back() != ';') {
					std::cerr << "Syntax error on line [" << line_cnt << "]: Line must end with a semicolon" << std::endl;
					return false;
				}
			}
		}
		line_cnt++;
	}

	

	return true;
}

/*-----------------------------------------validate Parmeters-----------------------------------------*/
bool	isValidIpAddress(const std::string &ip) {
	int dots = 0;
	for (size_t i = 0; i < ip.length(); ++i) {
		if (ip[i] == '.')
			dots++;
	}
	if (dots != 3) {
		return false;
	}

	std::string octet;
	int value = 0;
	int count = 0;

	for (size_t i = 0; i < ip.length(); ++i) {
		if (ip[i] == '.') {
			if (octet.empty() || octet.length() > 3) {
				return false;
			}
			value = 0;
			for (size_t j = 0; j < octet.length(); ++j){
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
			count++;
			octet.clear();
		} else {
			octet += ip[i];
		}
	}

	if (octet.empty() || octet.length() > 3) {
			return false;
	}
	value = 0;
	for (size_t j = 0; j < octet.length(); ++j){
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
	if (maxBodySize.back() != 'm') {
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

bool	isValidRouteData(const Route &routeData) {
	if (routeData.path[0] != '/') {
		std::cerr << "Invalid path: "<< routeData.path << ": Path must start with '/'" << std::endl;
		return false;
	}


	for(size_t i = 0; i < routeData.allowMethods.size(); ++i) {
		std::string method = routeData.allowMethods[i];
		if (method != "GET" && method != "POST" && method != "DELETE") {
			std::cerr << "Invalid method: Only GET, POST, and DELETE are allowed" << std::endl;
			return false;
		}
	}

	if (routeData.indexFile.length() < 5 || 
	routeData.indexFile.substr(routeData.indexFile.length() - 5) != ".html") {
		std::cerr << "Invalid index file: Index file must end with '.html'" << std::endl;
		return false;
	}

	return true;

}