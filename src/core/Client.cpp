#include "Client.hpp"

Client::Client(int fd, int epoll_fd)
: client_fd(fd), mode(HEADER_READING) {
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLOUT | EPOLLET;
	ev.data.ptr = this;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev) == -1) {
        throw std::runtime_error("Failed to add epoll");
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

const ServerConfig	&Client::getConfigDatum() const {
	return	this->configDatum;
}

void	Client::parseRequestHeader() {
	char	buffer[MAX_BUFEER];
	ssize_t count = read(this->client_fd, buffer, sizeof(buffer));

	if (count > 0) {
		req.setRawHeader(buffer);
		std::string& current_header = req.getRawHeader();
		size_t header_end = current_header.find("\r\n\r\n");

		if (header_end != std::string::npos) {
			std::string body_part = current_header.substr(header_end + 4);
			current_header = current_header.substr(0, header_end + 4);
			
			#ifdef DEBUG
			//std::cout << "header: \n" << current_header << std::endl;
			#endif

			if (req.requestParse(req.getRawHeader(), this->getConfigDatum()) == false) {
				std::cerr << "Bad Format: Header is not correct format" << std::endl;
			}

			if (!body_part.empty()) {
				req.setBody(body_part);
			}
			this->mode = BODY_READING;
		}
	}
}

void	Client::parseRequestBody() {
	if (req.checkBodyExist() == false)
		this->mode = WRITING;
	else {
		char	buffer[MAX_BUFEER];
		ssize_t count = read(this->client_fd, buffer, sizeof(buffer));
		if (count > 0) {
			this->req.appendBody(buffer);
		}
		//calucurate how many time to read by conten-length
		if (req.isBodyComplete()) {
			this->mode = WRITING;
		}
	}
}

void	Client::bindToConfig(std::vector<ServerConfig> &configData) {
	#ifdef DEBUG
 	//print_line(req);
	#endif
	std::map<std::string, std::string> header = this->req.getHeader();
	std::string hostValue;
	std::map<std::string, std::string>::iterator it = header.find("Host");
	if (it != header.end()) {
		hostValue = it->second;
		#ifdef DEBUG
		//std::cout << "Host: " << hostValue << std::endl;
		#endif
	} else {
		std::cerr << "None Header Host " << std::endl;
	}

	size_t colonPos = hostValue.find(':');
	std::string host;
	int port;
	if (colonPos != std::string::npos) {
		host = hostValue.substr(0, colonPos);
		port = std::strtol(hostValue.substr(colonPos + 1).c_str(), 0, 10);
	} else {
		host = hostValue;
		port = 8080;//should set default port number
	}

	for(std::vector<ServerConfig>::iterator iter = configData.begin(); iter != configData.end(); ++iter)
	{
		#ifdef DEBUG
		//std::cout << "ServerConfig: " << iter->serverName << ", " << iter->host << " : " << iter->listenPort << std::endl; 
		#endif
		if (iter->getServerName() == host) {
			this->configDatum = *iter;
		} else if (iter->getListenHost() == host && iter->getListenPort() == port) {
			this->configDatum = *iter;
		}
	}

	#ifdef DEBUG
	{
		//ServerConfig debug_data = this->getConfigDatum();
		//std::cout << "configDatum: " << "client fd: " << client_fd << ", " 
		//		<< "is_default: " << debug_data << ", "
		//		<< "listenPort: " << debug_data.listenPort << ", "
		//		<< "host: " << debug_data.host << ", "
		//		<< "serverName: " << debug_data.serverName << ", "
		//		<< "maxBodySize: " << debug_data.maxBodySize << std::endl;
	}
	#endif
}

void    Client::methodProc()
{
	Response msg;
	msg.createMessage(this->req, this->configDatum);
	msg.wirteMessage(this->client_fd);
}

void    Client::makeResponse() {
	#ifdef DEBUG
 	//print_line(req);
	//print_conf(configDatum);
	#endif
    this->methodProc();
}

