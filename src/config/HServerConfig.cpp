#include "HServerConfig.hpp"

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

void	HServerConfig::setListen(std::string& listen) {
	int	port;
	std::string host;

	if (listen.find(":") != std::string::npos) {
		int posColon = listen.find(":");
		host = listen.substr(0, posColon);
		std::stringstream ss(listen.substr(posColon));
		if (!(ss >> port) || !ss.eof()) {
			throw std::logic_error("[emerg] unexepcted paramaeter in \"listen\" directive");
		}
	} else {
		throw std::logic_error("[emerg] unexepcted paramaeter in \"listen\" directive");
	}
	if (isValidIpAddress(host) != true) {
		throw std::logic_error("[emerg] unexepcted paramaeter in \"listen\" directive");
	}
	if (port < 0 || port > 65535) {
		throw std::logic_error("[emerg] unexepcted paramaeter in \"listen\" directive");
	}
	this->host = host;
	this->listenPort = port;
}

void	HServerConfig::setServerName(std::string& name) {
	this->serverName = name;
}

void	HServerConfig::setErrorPage(std::string& page) {

}

void	HServerConfig::addLocation(const std::vector<std::string>& tokens, size_t& index) {

}