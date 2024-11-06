#include "Client.hpp"

Client::Client(int fd, int epoll_fd)
: client_fd(fd), mode(ClientMode::READING) {
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
    return client_fd;
}

// void    Client::handleClientEvent(uint32_t events) {
    
// }