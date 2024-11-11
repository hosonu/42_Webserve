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
    req.setRawHeader(buffer);
    // std::cout << req.getRawHeader() << std::endl;
    std::cout << "request fd:" << client_fd << std::endl;
	req.requestParse(req.getRawHeader());
}

void    Client::makeResponse() {
    std::cout << req.getRawHeader() << std::endl;
    std::cout << "response fd:" << client_fd << std::endl;
    req.methodProc(client_fd);
}
