#include "Server.hpp"

Server::Server(Config &configs) : configData(configs.getServerConfigs()) {
}

void	Server::setServer() {
	epoll_fd_ = epoll_create1(0);
	if (epoll_fd_ == -1) {
		throw std::runtime_error("Failed to create epoll file descriptor");
	}
	for(std::vector<ServerConfig>::iterator it = configData.begin(); it != configData.end(); ++it) {
		if (it->getDefault() == true) {
			Socket socket(it->getListenHost(), it->getListenPort());
			if (socket.setNonBlocking(socket.getFd()) == false)
				throw std::runtime_error("Failed to set non blocking mode");
			if (socket.bind() == false) {
				throw std::runtime_error("Failed to bind socket");
			}
			if (socket.listen() == false) {
				throw std::runtime_error("Failed to listen on socket");
			}
			struct epoll_event ev;
			ev.events = EPOLLIN | EPOLLOUT;
			ev.data.fd = socket.getFd();
			if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, socket.getFd(), &ev) == -1) {
				throw std::runtime_error("Failed to add to epoll");
			}
			socket_.push_back(socket);
		}
	}
}

Server::~Server() {
	closeServer();
	configData.clear();
	socket_.clear();
	client_.clear();
}

void	Server::run() {
	while(true) {
		time_t current_time = time(NULL);
		int n = epoll_wait(epoll_fd_, events_, MAX_EVENTS, EPOLL_TIMEOUT_MS);
		if (n == -1) {
			throw std::runtime_error("epoll_wait failed");
		} else if (n == 0) {
			continue;
		}
		for (std::list<Client>::iterator it = client_.begin(); it != client_.end();) {
			if (it->isTimedOut(current_time, CLIENT_TIMEOUT_SEC)) {
				#ifdef DEBUG
				std::cout << "Client timed out: " << it->getClientFd() << ", size: " << client_.size() << std::endl;
				std::cout << "timeout detected" << std::endl;
				std::cout << "timeout type:" << it->getClientMode() << std::endl;
				#endif
				if (it->getClientMode() == CGI_READING) {
					it->end_timeoutCGI();
				} else {
					//removeClient(it->getClientFd());
					//continue;
					int fd = it->getClientFd();
					it = client_.erase(it);
					epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, NULL);
					close(fd);
				}
			} else {
				++it;
			}
		}
		for (int i = 0; i < n; ++i) {
			int fd = events_[i].data.fd;
			bool handled = false;
			for (size_t i = 0; i < socket_.size(); ++i) {
				if (socket_[i].getFd() == fd) {
					acceptNewConnection(socket_[i]);
					handled = true;
					break;
				}
			}
			if (!handled) {
				Client* client = static_cast<Client*>(events_[i].data.ptr);
				if (client != NULL) {
					if (events_[i].events & EPOLLIN) {
						#ifdef DEBUG
						std::cout << "client: " << client->getClientFd() << " mode: " << client->getClientMode() << " in handle request" <<  std::endl;
						#endif
						HandleRequest(*client);
					}
					else if (events_[i].events & EPOLLOUT) 
					{
						if (client->getClientMode() == CGI_READING) {
							client->readCGI();
						} else {
							#ifdef DEBUG
							std::cout << "client: " << client->getClientFd() << " mode: " << client->getClientMode() << " in handle response" <<  std::endl;
							#endif
							HandleResponse(*client);
						}
					}
				}
			}
		}
	}
}

void	Server::acceptNewConnection(Socket& listen_socket) {
	int client_fd = listen_socket.accept();
	if (client_fd == -1) {
		std::cerr << "Failed to accept listen_fd" << std::endl;
		return;
	}
	listen_socket.setNonBlocking(client_fd);

	client_.push_back(Client(client_fd, epoll_fd_));
    Client& new_client = client_.back();
    new_client.updateActivity();
    
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLOUT;
    ev.data.ptr = &new_client;
    if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, client_fd, &ev) == -1) {
        client_.pop_back();
        throw std::runtime_error("Failed to add epoll");
    }
	#ifdef DEBUG
	std::cout << "Make new client: " << client_fd << std::endl;
	#endif
}

void	Server::HandleRequest(Client &client) {
	if (client.getClientMode() == HEADER_READING) {
		client.parseRequestHeader(this->configData);
		client.updateActivity();
	}
	if (client.getClientMode() == BODY_READING) {
		client.parseRequestBody();
		client.updateActivity();
	}
}

void	Server::HandleResponse(Client &client) {
	if (client.getClientMode() == WAITING || client.getClientMode() == WRITING) {
		client.makeResponse();
		client.updateActivity();
	}
	if (client.getClientMode() == CLOSING) {
		removeClient(client.getClientFd());
	}
}

void	Server::removeClient(int client_fd) {
	#ifdef DEBUG
	std::cout << "Remove client: " << client_fd << " size: " << client_.size() << std::endl;
	#endif
	if (epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, client_fd, NULL) == -1) {
		std::cerr << "Failed to remove epoll" << std::endl;
	}
	close(client_fd);
	for (std::list<Client>::iterator it = client_.begin(); it != client_.end();) {
		if (it->getClientFd() == client_fd) {
			#ifdef DEBUG
			std::cout << "Ex Remove client: " << client_fd << std::endl;
			#endif
			it = client_.erase(it);
			break;
		} else {
			++it;
		}
	}
}

void	Server::closeServer() {
	close(epoll_fd_);
	for (std::list<Client>::iterator it = client_.begin(); it != client_.end(); ++it) {
		close(it->getClientFd());
	}
	for (std::vector<Socket>::iterator it = socket_.begin(); it != socket_.end(); ++it) {
		close(it->getFd());
	}
}
