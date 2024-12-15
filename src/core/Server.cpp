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
		int n = epoll_wait(epoll_fd_, events_, MAX_EVENTS, 1);
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
				std::cout << "timeout detected" << std::endl;
				std::cout << "timeout type:" << it->getClientMode() << std::endl;
				if (it->getClientMode() == CGI_READING)
					it->end_timeoutCGI();
				else
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
					acceptNewConnection(socket_[i]);
					#ifdef DEBUG
					std::cout << " , fd: " << fd << std::endl;
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
					else if (events_[i].events & EPOLLOUT) 
					{
						if (client->getClientMode() == CGI_READING) 
							client->readCGI();
						else 
							HandleResponse(*client);
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
	
 	if (client_.size() == client_.capacity()) {
            client_.reserve(client_.size() + 10);
    }

	//client_.reserve(client_.size() + 1);

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
	//std::cout << "mode: " << client.getClientMode() << " , in HandleRequest" <<  std::endl;
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

void	Server::HandleResponse(Client &client) {
	//std::cout << "mode: " << client.getClientMode() << " , in HandleResponse" << std::endl;
	if (client.getClientMode() == WAITING || client.getClientMode() == WRITING) {
		#ifdef DEBUG
		std::cout << "WRITING NOW" << std::endl;
		#endif
		//client.setMode(CLOSING);
		client.makeResponse();
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
