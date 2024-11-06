#include "Server.hpp"

Server::Server(Config &configs) : configData(configs.getServerConfig()) {
}

void	Server::setServer() {
	epoll_fd_ = epoll_create1(0);
	if (epoll_fd_ == -1) {
		throw std::runtime_error("Failed to create epoll file descriptor");
	}

	for(std::vector<ServerConfig>::iterator it = configData.begin(); it != configData.end(); ++it) {
		Socket socket(it->host, it->listenPort);
		if (socket.setNonBlocking(socket.getFd()) == false)
			throw std::runtime_error("Failed to set non blocking mode");
		if (socket.bind() == false) {
			throw std::runtime_error("Failed to bind socket");
		}
		if (socket.listen() == false) {
			throw std::runtime_error("Failed to listen on socket");
		}
		struct epoll_event ev;
		ev.events = EPOLLIN | EPOLLET;
		ev.data.fd = socket.getFd();
		if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, socket.getFd(), &ev) == -1) {
			throw std::runtime_error("Failed to add to epoll");
		}
		socket_.push_back(socket);
	}
}

Server::~Server() {
    // close(epoll_fd_);
	//close fdfdfdfdf
}

void	Server::run() {
	while(true) {
		int n = epoll_wait(epoll_fd_, events_, MAX_EVENTS, -1);
		if (n == -1) {
			throw std::runtime_error("epoll_wait failed");
		}
		for (int i = 0; i < n; ++i) {
			int fd = events_[i].data.fd;
			//listen fd の処理
			for (size_t i = 0; i < socket_.size(); ++i) {
				if (socket_[i].getFd() == fd) {
					int client_fd = acceptNewConnection(socket_[i]);
					Client client(client_fd, epoll_fd_);
					client_.push_back(client);
				}
			}
			//client fd の処理
			for (size_t i = 0; i< client_.size(); ++i) {
				if (client_[i].getClientFd() == fd) {
					if (events_[i].events & EPOLLIN) {
						// readの処理
						//buffer保存
						HandleRequest(client_[i]);
					}
					else if (events_[i].events & EPOLLOUT) {
						//writeの処理
						//buffer保存
						HandleResponse(client_[i]);
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

	//configの情報を引き渡す必要があるかもしれない。
	return client_fd;
}

void	Server::HandleRequest(Client client) {
	client.setMode(ClientMode::READING);
	char buffer[MAX_BUFEER];
	ssize_t count;
	request req;
	std::string	rawReq;

	//readが失敗した時の対応、bufferの保存
	count = read(client.getClientFd(), buffer, sizeof(buffer));
	if (count > 0) {
		rawReq = buffer;
		req.requestParse(rawReq);
	}
	//readが終了したあとの挙動
	//modeの変更？
	if (count == 0) {
		client.setMode(ClientMode::WRITING);//仮に設定
		close(client.getClientFd());
	}
}

void	Server::HandleResponse(Client client) {
	// client.setMode(ClientMode::WRITING);

	//関数内で処理を変更
	//bufferの保存、ファイルオフセットの保存
	request req;
	req.methodProc(client.getClientFd());
}

// void	Server::handleClient(int client_fd) {
// 	//add the line pocessing cliant connnection
// 	char buffer[1024];
// 	ssize_t count = read(client_fd, buffer, sizeof(buffer));
// 	request req;
// 	std::string rawReq;
// 	if (count == -1) {
// 		if (errno != EAGAIN) {
// 			close(client_fd);
// 		}
// 	} else if (count == 0) {
// 		close(client_fd);
// 	} else {
// 		//add the line processing accepted date
// 		// std::cout << "Received: " << std::string(buffer, count) << std::endl;
// 		rawReq = buffer;
// 		req.requestParse(rawReq);
// 		req.methodProc(client_fd);
// 		// //test
// 		// print_line(req);
// 	}
// }

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

