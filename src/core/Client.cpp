#include "Client.hpp"

Client::Client(int fd, int epoll_fd)
: client_fd(fd), mode(ClientMode::HEADER_READING) {
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLOUT | EPOLLET;
    ev.data.fd = client_fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev) == -1) {
        // thorow std::runtime_error("Failed to add epoll");
        std::cerr << "" << std::endl;
    }
}

void    Client::setMode(ClientMode mode) {
    this->mode = mode;
}

int Client::getClientFd() const{
    return this->client_fd;
}

const ClientMode&  Client::getClientMode() const {
    return  this->mode;
}

void	Client::parseRequestHeader() {
	ssize_t count;
	char	buffer[MAX_BUFEER];
	count = read(this->client_fd, buffer, sizeof(buffer));
	if (count > 0) {
		req.setRawHeader(buffer);
		if (req.requestParse(req.getRawHeader()) == false) {
			std::cerr << "Bad Format: Header is not correct format" << std::endl;
		}
	}
	if (req.getCgMode() == true) {
		this->mode = ClientMode::BODY_READING;
		req.setCgMode(false);
	}
	//config dataをclient_fdに紐づける
}

void	Client::parseRequestBody() {
	if (req.checkBodyExist() == false)
		this->mode = ClientMode::WRITING;
	else {
		ssize_t count;
		char	buffer[MAX_BUFEER];
		count = read(this->client_fd, buffer, sizeof(buffer));
		if (count > 0) {
			this->req.setBody(buffer);
		}
	}
}

void	Client::bindToConfig(std::vector<ServerConfig> &configData) {

	std::map<std::string, std::string> header = this->req.getHeader();
	std::string hostValue;
	std::map<std::string, std::string>::iterator it = header.find("Host");
	if (it != header.end()) {
		hostValue = it->second;
		#ifdef DEBUG
		std::cout << "Host: " << hostValue << std::endl;
		#endif
	}

	size_t colonPos = hostValue.find(':');
	std::string host;
	int port;
	if (colonPos != std::string::npos) {
		host = hostValue.substr(0, colonPos);
		port = atoi(hostValue.substr(colonPos + 1).c_str());
	} else {
		host = hostValue;
		port = 8080;//should set default port number
	}

	for(std::vector<ServerConfig>::iterator iter = configData.begin(); iter != configData.end(); ++iter)
	{
		if (iter->serverName == host) {
			this->configDatum = *iter;
		} else if (iter->host == host && iter->listenPort == port) {
			this->configDatum = *iter;
		}
	}

	#ifdef DEBUG
	std::cout << "configDatum: " 
	          << "is_default: " << configDatum.is_default << ", "
	          << "listenPort: " << configDatum.listenPort << ", "
	          << "host: " << configDatum.host << ", "
	          << "serverName: " << configDatum.serverName << ", "
	          << "maxBodySize: " << configDatum.maxBodySize << std::endl;
	#endif
}

void    Client::makeResponse() {
	#ifdef DEBUG
 	//print_line(req);
	#endif
    req.methodProc(client_fd);
}

