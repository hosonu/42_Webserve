#include "Server.hpp"

Server::Server(Config &configs) : configData(configs.getServerConfig()) {
}

void	Server::setServer() {
	epoll_fd_ = epoll_create1(0);
	if (epoll_fd_ == -1) {
		throw std::runtime_error("Failed to create epoll file descriptor");
	}

	for(std::vector<ServerConfig>::iterator it = configData.begin(); it != configData.end(); ++it) {

		/*debugginggggg*/
		std::cout << "listen: host " << it->host << ", port " << it->listenPort << std::endl;
		std::cout << "sever_name: " << it->serverName << std::endl;
		std::cout << "errorpage: " << it->errorPages[404] << " " << it->errorPages[502] << std::endl;
		std::cout << "CMBS: " << it->maxBodySize << std::endl;
		std::cout << "allow_methods: " << it->routeData.allowMethods[1] << std::endl;
		std::cout << "autoindex: " << it->routeData.autoindex << std::endl;
		std::cout << "root: " << it->routeData.root << std::endl;
		std::cout << "index: " << it->routeData.indexFile << std::endl;

		Socket socket(it->host, it->listenPort);
		if (socket.setNonBlocking(socket.getFd()) == false)
			throw std::runtime_error("fuck it");
		if (socket.bind() == false) {
			throw std::runtime_error("Failed to bind socket");
		}
		if (socket.listen() == false) {
			throw std::runtime_error("Failed to listen on socket");
		}
		struct epoll_event ev;
		ev.events = EPOLLIN;
		ev.data.fd = socket.getFd();
		if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, socket.getFd(), &ev) == -1) {
			throw std::runtime_error("Failed to add to epoll");
		}
		socket_.push_back(socket);
	}

	events_.resize(MAX_EVENTS);
}

Server::~Server() {
    // close(epoll_fd_);
	//close fdfdfdfdf
}

void	Server::run() {
	while(true) {
		int n = epoll_wait(epoll_fd_, events_.data(), events_.size(), -1);
		if (n == -1) {
			throw std::runtime_error("epoll_wait failed");
		}

		for (int i = 0; i < n; ++i) {
			int fd = events_[i].data.fd;

			bool found = false;
			for (size_t i = 0; i < socket_.size(); ++i) {
				if (socket_[i].getFd() == fd) {
					acceptNewConnection(socket_[i]);
					found = true;
					break;
				}
			}
			if (!found) {
				std::cout << fd << std::endl;
				handleClient(fd);//this fd 
			}
		}
	}
}

void	Server::acceptNewConnection(Socket& listen_socket) {
	int client_fd = listen_socket.accept();
	if (client_fd == -1) {
		std::cerr << "hello" << std::endl;
		return;
	}

	// setNonBlocking_cs(client_fd);
	listen_socket.setNonBlocking(client_fd);

	struct epoll_event ev;
	ev.events = EPOLLIN | EPOLLET;
	ev.data.fd = client_fd;
	if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, client_fd, &ev) == -1) {
		//error message? or throw?
		std::cerr << "Failed to add client socket to epoll: " << strerror(errno) << std::endl;
		close(client_fd);
		return;
	}

	// client_configs_[client_fd] = &config;//config data for fd
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

//setNonBlocking for clienat socket いらない？
// void Server::setNonBlocking_cs(int fd) {
// 	int flags = fcntl(fd, F_GETFL, 0);
// 	if (flags == -1) {
// 		throw std::runtime_error("Failed to get file descriptor flags");
// 	}
// 	if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
// 		throw std::runtime_error("Failed to set non-blocking mode");
// 	}
// }

