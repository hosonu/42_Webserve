#include "Server.hpp"

// Server::Server(const std::string& host, int port)
// : socket_(host, port) {
// 	if (socket_.bind() == false) {
// 		throw std::runtime_error("Failed to bind socket");
// 	}
// 	if (socket_.listen() == false) {
// 		throw std::runtime_error("Failed to listen on socket");
// 	}

// 	epoll_fd_ = epoll_create1(0);
// 	if (epoll_fd_ == -1) {
// 		throw std::runtime_error("Failed to create epoll file descriptor");
// 	}

// 	struct epoll_event event;
// 	event.events = EPOLLIN;
// 	event.data.fd = socket_.getFd();
// 	if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, socket_.getFd(), &event) == -1) {
// 		throw std::runtime_error("Failed to add socket to epoll");
// 	}

// 	events_.resize(MAX_EVENTS);//init size of event list
// }

Server::Server(const std::vector<ServerConfig> &configs) {

	epoll_fd_ = epoll_create1(0);
	if (epoll_fd_ == -1) {
		throw std::runtime_error("Failed to create epoll file descriptor");
	}
	for (std::vector<ServerConfig>::const_iterator it = configs.begin(); it != configs.end(); ++it) {
        Socket socket(it->host, it->listenPort);
        socket.setNonBlocking();
        if (!socket.bind()) {
            throw std::runtime_error("Failed to bind socket");
        }
        if (!socket.listen()) {
            throw std::runtime_error("Failed to listen on socket");
        }

		struct epoll_event ev;
		ev.events = EPOLLIN;
		ev.data.fd = socket.getFd();
		if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, socket.getFd(), &ev) == -1) {
			throw std::runtime_error("Failed to add to epoll");
		}

		events_.resize(MAX_EVENTS);//init size of event list
        // server_instances_.push_back(ServerInstance{socket, *it});
		ServerInstance instance;
		instance.socket_ = socket;
		instance.config = *it;
		server_instances_.push_back(instance);

		
		std::cout << it->host << " : " << it->listenPort << ";" << std::endl;
    }
}

Server::~Server() {
    // close(epoll_fd_);
}

void	Server::run() {
	while(true) {
		int n = epoll_wait(epoll_fd_, events_.data(), events_.size(), -1);
		if (n == -1) {
			throw std::runtime_error("epoll_wait failed");
		}

		for (int i = 0; i < n; ++i) {
			int fd = events_[i].data.fd;

			std::vector<ServerInstance>::iterator it;
			for (it = server_instances_.begin(); it != server_instances_.end(); ++it) {
				std::cout << it->config.host << " : " << it->config.listenPort << ";" << std::endl;
				if (it->socket_.getFd() == fd) {
					acceptNewConnection(it->socket_, it->config);//should output error messages 
					// break;
				} else {
					handleClient(fd);

				}

			}
		}
			// for (int i = 0; i < n; ++i) {
			// 	std::vector<ServerInstance>::iterator it;
			// 	it = server_instances_.begin();
			// 	if (events_[i].data.fd == it->socket_.getFd()) {
			// 		int client_fd = it->socket_.accept();
			// 		if (client_fd >= 0) {
			// 			struct epoll_event event;
			// 			event.events = EPOLLIN | EPOLLET;
			// 			event.data.fd = client_fd;
			// 			if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, client_fd, &event) == -1) {
			// 				close(client_fd);
			// 			}
			// 		}
			// 	} else {
			// 		int client_fd = events_[i].data.fd;
			// 		handleClient(client_fd);
			// 	}
			// std::vector<ServerInstance>::iterator it;
			// it = server_instances_.begin();
			// std::cout << it->config.host << " : " << it->config.listenPort << ";" << std::endl;
			// if (it->socket_.getFd() == fd) {
			// 	acceptNewConnection(it->socket_, it->config);//should output error messages 
			// 	break;
			// }
			// if (it == server_instances_.end()) {
			// 	handleClient(fd);
			// }

			
		// }
	}
}

void	Server::acceptNewConnection(Socket& listen_socket, const ServerConfig& config) {
	int client_fd = listen_socket.accept();
	if (client_fd == -1) {
		std::cerr << "hello" << std::endl;
		return;
	}

	setNonBlocking_cs(client_fd);

	struct epoll_event ev;
	ev.events = EPOLLIN | EPOLLET;
	ev.data.fd = client_fd;
	if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, client_fd, &ev) == -1) {
		//error message? or throw?
		std::cerr << "Failed to add client socket to epoll: " << strerror(errno) << std::endl;
		close(client_fd);
		return;
	}

	client_configs_[client_fd] = &config;
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

//setNonBlocking for clienat socket
void Server::setNonBlocking_cs(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}
