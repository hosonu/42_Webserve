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
			int statusCode = std::strtol(tokens[i].c_str(), 0, 10);
			currentServer.errorPages[statusCode] = path;
	}
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

bool checkBraceFlags(std::stringstream &file) {
	std::string line;
	int	cnt_line = 1;
	int braceCount = 0;
	bool serverFlag = false;
	bool errorPageFlag = false;
	bool clientMaxBodySizeFlag = false;
	bool locationFlag = false;

	std::stringstream tmp;
    tmp << file.rdbuf();
    file.clear();
    file.seekg(0);
	
	while(std::getline(tmp, line)) {
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
	// std::cout << it->first << std::endl;
	for (it = errorPages.begin(); it != errorPages.end(); ++it) {
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