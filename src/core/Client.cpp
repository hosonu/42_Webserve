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

void    Client::makeResponse() {
	#ifdef DEBUG
 	print_line(req);
	#endif
    req.methodProc(client_fd);
}
