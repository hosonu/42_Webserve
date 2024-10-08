#include "ServerConfig.hpp"

ServerConfig::ServerConfig() : listenPort(0), host("") {}

ServerConfig::~ServerConfig() {}

void	ServerConfig::setHost(const std::string &host) {
	this->host = host;
}

void	ServerConfig::setListenPort(int port) {
	this->listenPort = port;
}

std::string ServerConfig::getHost() const {
	return this->host;
}

int	ServerConfig::getListenPort() const {
	return this->listenPort;
}