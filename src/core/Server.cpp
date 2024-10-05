#include "Server.hpp"

Server::Server(const std::string& host, int port)
: socket_(host, port) {
	if (socket_.bind() == false) {
		throw std::runtime_error("Failed to bind socket");
	}
	if (socket_.listen() == false) {
		throw std::runtime_error("Failed to listen on socket");
	}

	epoll_fd_ = epoll_create1(0);
	if (epoll_fd_ == -1) {
		throw std::runtime_error("Failed to create epoll file descriptor");
	}

	struct epoll_event event;
	event.events = EPOLLIN;
	event.data.fd = socket_.getFd();
	if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, socket_.getFd(), &event) == -1) {
		throw std::runtime_error("Failed to add socket to epoll");
	}

	events_.resize(MAX_EVENTS);//init size of event list
}

Server::~Server() {
}

void	Server::run() {
	while(true) {
		int n = epoll_wait(epoll_fd_, events_.data(), events_.size(), -1);
		if (n == -1) {
			throw std::runtime_error("epoll_wait failed");
		}

		for (int i = 0; i < n; ++i) {
			if (events_[i].data.fd == socket_.getFd()) {
				int client_fd = socket_.accept();
				if (client_fd >= 0) {
					struct epoll_event event;
					event.events = EPOLLIN | EPOLLET;
					event.data.fd = client_fd;
					if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, client_fd, &event) == -1) {
						close(client_fd);
					}
				}
			} else {
				int client_fd = events_[i].data.fd;
				handleClient(client_fd);
			}
		}
	}
}

void	Server::handleClient(int client_fd) {
	//add the line pocessing cliant connnection
	char buffer[1024];
	ssize_t count = read(client_fd, buffer, sizeof(buffer));
	if (count == -1) {
		if (errno != EAGAIN) {
			close(client_fd);
		}
	} else if (count == 0) {
		close(client_fd);
	} else {
		//add the line processing accepted date
		std::cout << "Received: " << std::string(buffer, count) << std::endl;
	}
}
