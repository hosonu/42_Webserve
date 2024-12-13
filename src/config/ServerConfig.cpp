#include "ServerConfig.hpp"

ServerConfig::ServerConfig()
: is_default(false), listenPort(80), host("localhost"), serverName(""), maxBodySize(""), locations() {}

void	ServerConfig::setDefault(bool flag) {
	this->is_default = flag;
}

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

void	ServerConfig::setListen(std::string& listen) {
	int	port;
	std::string host;

	if (listen.find(":") != std::string::npos) {
		int posColon = listen.find(":");
		host = listen.substr(0, posColon);
		std::stringstream ss(listen.substr(posColon + 1));
		if (!(ss >> port) || !ss.eof()) {
			throw std::logic_error("[emerg] unexepcted paramaeter in \"listen\" directive");
		}
	} else {
		throw std::logic_error("[emerg] unexepcted paramaeter in \"listen\" directive, should : ");
	}
	if (isValidIpAddress(host) != true) {
		throw std::logic_error("[emerg] unexepcted ip paramaeter in \"listen\" directive : " + host);
	}
	if (port < 0 || port > 65535) {
		throw std::logic_error("[emerg] unexepcted port paramaeter in \"listen\" directive" + customToString(port));
	}
	this->host = host;
	this->listenPort = port;
}

void	ServerConfig::setServerName(const std::string& name) {
	this->serverName = name;
}

void	ServerConfig::setErrorPage(const std::vector<std::string>& tokens, size_t& i) {
	i += 1;
	size_t	index = i;
	while (tokens[index] != ";")
		index++;
	std::string path = tokens[index - 1];
	if (path.length() < 5 || path.substr(path.length() - 5) != ".html"){
		throw std::logic_error("[emerg] unexepcted errorPage path paramaeter in \"listen\" directive : " + path);
	}
	for (; i < index - 1; i++) {
		for (size_t j = 0; j < tokens[i].length(); ++j) {
			if (!std::isdigit(tokens[i][j])) {
				throw std::logic_error("[emerg] unexepcted status code paramaeter in \"listen\" directive : " + tokens[i]);
			}
		}
		int statusCode = std::strtol(tokens[i].c_str(), 0, 10);
		if (statusCode < 300 || statusCode > 599) {
			throw std::logic_error("[emerg] unexepcted status code paramaeter in \"listen\" directive");
		}
		if (errorPages[statusCode].empty() == false) {
			throw std::logic_error("[emerg] unexepcted status code paramaeter in \"listen\" directive");
		}
		this->errorPages[statusCode] = path;
	}
}

void	ServerConfig::setClientMaxBodySize(const std::string& size) {
	if (size.length() < 2) {
		throw std::logic_error("[emerg] unexepcted clientMaxBodySize paramaeter in \"listen\" directive : " + size);
	}
	if (size[size.length() - 1] != 'm' && size[size.length() - 1] != 'k' && size[size.length() - 1] != 'g') {
		throw std::logic_error("[emerg] unexepcted clientMaxBodySize paramaeter in \"listen\" directive : " + size);
	}
	char unit = size[size.length() - 1];
	std::string num = size.substr(0, size.length() - 1);
	for (size_t i = 0; i < num.size(); ++i) {
		if (!std::isdigit(num[i])) {
			throw std::logic_error("[emerg] unexepcted clientMaxBodySize paramaeter in \"listen\" directive");
		}
		if (num[0] == 0 && i > 1) {
			throw std::logic_error("[emerg] unexepcted clientMaxBodySize paramaeter in \"listen\" directive");
		}
	}
	
	size_t maxDigits;
	switch (unit) {
		case 'k':
			maxDigits = 13;
			break;
		case 'm':
			maxDigits = 10;
			break;
		case 'g':
			maxDigits = 7;
			break;
	}
	if (num.length() > maxDigits) {
		throw std::logic_error("[emerg] unexepcted clientMaxBodySize paramaeter in \"listen\" directive");
	}

	this->maxBodySize = size;
}

void	ServerConfig::addLocation(const std::vector<std::string>& tokens, size_t& index) {
	index += 1;
	Location	currentLocation;
	size_t length = 0;
	currentLocation.setPath(tokens[index]);
	for (size_t i = index + 1; i < tokens.size(); ++i) {
		if (tokens[i] == "allow_methods") {
			i++;
			while(tokens[i] != ";") {
				currentLocation.setAllowMethods(tokens[i]);
				i++;
			}
		} else if (tokens[i] == "autoindex") {
			currentLocation.setAutoindex(tokens[i + 1]);
			i++;
		} else if (tokens[i] == "root") {
			currentLocation.setRoot(tokens[i + 1]);
			i++;
		} else if (tokens[i] == "index") {
			currentLocation.setIndexFile(tokens[i + 1]);
			i++;
		} else if (tokens[i] == "return") {
			currentLocation.setReturnPath(tokens[i + 1]);
			i++;
		} else if (tokens[i] == "isCGI") {
			currentLocation.setCGI(tokens[i + 1]);
		}
		else if (tokens[i] == "}") {
			length = i;
			break;
		}

	}
	index += (length - index);
	locations.push_back(currentLocation);
}

void ServerConfig::sortLocations() {
	std::sort(locations.begin(), locations.end(), LocationCompare());
}

bool	ServerConfig::getDefault() const {
	return this->is_default;
}

std::string ServerConfig::getListenHost() const {
    return this->host;
}

int ServerConfig::getListenPort() const {
    return this->listenPort;
}

std::string ServerConfig::getServerName() const {
    return this->serverName;
}

std::string ServerConfig::getMaxBodySize() const {
    return this->maxBodySize;
}

const std::map<int, std::string>& ServerConfig::getErrorPages() const {
    return this->errorPages;
}

const std::vector<Location>& ServerConfig::getLocations() const {
    return this->locations;
}

