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
		for (std::vector<Client>::iterator it = client_.begin(); it != client_.end();) {
			if (it->isTimedOut(current_time, CLIENT_TIMEOUT_SEC)) {
				#ifdef DEBUG
				std::cout << "Client timed out: " << it->getClientFd() << ", size: " << client_.size() << std::endl;
				#endif
				removeClient(it->getClientFd());
				continue;
			} else {
				++it;
			}
		}

		for (int i = 0; i < n; ++i) {
			int fd = events_[i].data.fd;
			#ifdef DEBUG
			std::cout << "events: " << events_[i].events << std::flush;
			#endif
			//manage listen fd
			bool handled = false;
			for (size_t i = 0; i < socket_.size(); ++i) {
				if (socket_[i].getFd() == fd) {
					int client_fd = acceptNewConnection(socket_[i]);
					Client client(client_fd, epoll_fd_);
					client.updateActivity();
					client_.push_back(client);
					#ifdef DEBUG
					std::cout << " , fd: " << fd << std::endl;
					std::cout << "Make new client: " << client_fd << std::endl;
					#endif
					handled = true;
					break;
				}
			}
			if (!handled) {
				Client* client = static_cast<Client*>(events_[i].data.ptr);
				#ifdef DEBUG
				std::cout << " , fd: " << client->getClientFd() << std::endl;
				#endif
				if (client != NULL) {
					if (events_[i].events & EPOLLIN) {
						HandleRequest(*client);
					}
					if (events_[i].events & EPOLLOUT) {
						if (client->getClientMode() == CGI_READING) {
							client->readCGI();
							client->updateActivity();
						} 
						HandleResponse(*client);
					}
				}
			}
		}
	}
}

int	Server::acceptNewConnection(Socket& listen_socket) {
	int client_fd = listen_socket.accept();
	if (client_fd == -1) {
		std::cerr << "Failed to accept listen_fd" << std::endl;
		return -1;
	}
	listen_socket.setNonBlocking(client_fd);
	return client_fd;
}

void	Server::HandleRequest(Client &client) {
	std::cout << "mode: " << client.getClientMode() << " , in HandleRequest" <<  std::endl;
	if (client.getClientMode() == HEADER_READING) {
		#ifdef DEBUG
		std::cout << "HEADER_READING NOW" << std::endl;
		#endif
		client.parseRequestHeader(this->configData);
		client.updateActivity();
		//client.bindToConfig(this->configData);
	}
	if (client.getClientMode() == BODY_READING) {
		#ifdef DEBUG
		std::cout << "BODY_READING NOW" << std::endl;
		#endif
		client.parseRequestBody();
		client.updateActivity();
	}
}

void	Server::HandleResponse(Client &	client) {
	std::cout << "mode: " << client.getClientMode() << " , in HandleResponse" << std::endl;
	if (client.getClientMode() == WAITING || client.getClientMode() == WRITING) {
		#ifdef DEBUG
		std::cout << "WRITING NOW" << std::endl;
		client.makeResponse();
		#endif
		//client.setMode(CLOSING);
		client.updateActivity();
	}
	if (client.getClientMode() == CLOSING) {
		#ifdef DEBUG
		std::cout << "REMOVING NOW" << std::endl;
		#endif
		removeClient(client.getClientFd());
	}
}

void	Server::removeClient(int client_fd) {
	if (epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, client_fd, NULL) == -1) {
        #ifdef DEBUG
        std::cerr << "Failed to remove client_fd from epoll: " << client_fd
                  << " , error: " << strerror(errno) << std::endl;
        #endif
    }
	close(client_fd);
	for (std::vector<Client>::iterator it = client_.begin(); it != client_.end();) {
		if (it->getClientFd() == client_fd) {
			#ifdef DEBUG
			std::cout << "remove client : " << it->getClientFd() << ", client size : " << client_.size() << std::endl;
			#endif
			it = client_.erase(it);
			break;
		} else {
			++it;
		}
	}
}
