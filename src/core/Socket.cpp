#include "Socket.hpp"

Socket::Socket() {}

Socket::Socket(const std::string& host, int port) 
: fd_(-1), host_(host), port_(port), is_listening_(false) {
	fd_ = socket(AF_INET, SOCK_STREAM, 0);
	if (fd_ < 0) {
		return ; //error management
	}

	memset(&address_, 0, sizeof(address_));
	address_.sin_family = AF_INET;
	address_.sin_port = htons(port_);
	if (host_ == "0.0.0.0") {
		address_.sin_addr.s_addr = INADDR_ANY;
	} else {
		// if (inet_pton(AF_INET, host_.c_str(), &address_.sin_addr) <= 0) {
		// 	close();
		// 	return;
		// }
		return ;
	}
}

Socket::Socket(const Socket &src) {
	fd_ = src.fd_;
	host_ = src.host_;
	port_ = src.port_;
	is_listening_ = src.is_listening_;
	address_ = src.address_;
}

Socket& Socket::operator=(const Socket &rhs) {
	if (this != &rhs) {
		fd_ = rhs.fd_;
		host_ = rhs.host_;
		port_ = rhs.port_;
		is_listening_ = rhs.is_listening_;
		address_ = rhs.address_;
	}
	return *this;
}

Socket::~Socket() {
	close();
}

bool	Socket::bind() {
	int opt = 1;
	if (setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		return false; // add error message?
	}

	if (::bind(fd_, (struct sockaddr *)&address_, sizeof(address_)) < 0) {
		return false; // add error message?
	}
	return true;
}

bool	Socket::listen(int backlog) {
	if(::listen(fd_, backlog) < 0) {
		return false; // add error message?
	}
	is_listening_ = true;
	return true;
}

int Socket::accept() {
	struct sockaddr_in client_addr;
	socklen_t addr_len = sizeof(client_addr);

	int client_fd = ::accept(fd_, (struct sockaddr*)&client_addr, &addr_len);
	if (client_fd < 0) {
		if (errno != EAGAIN && errno != EWOULDBLOCK) {
			// setLastError("accept failed: " + std::string(strerror(errno)));
			//error message?
			write(2, "hello\n", 6);
		}
		return -1;
	}
	return client_fd;
}

bool	Socket::setNonBlocking() {
	int flags = fcntl(fd_, F_GETFL, 0);
	if (flags == -1) {
		return false; //add error message?
	}
	if (fcntl(fd_, F_SETFL, flags | O_NONBLOCK) == -1) {
		return false; //add error message?
	}

	return true;
}

int	Socket::getFd() const	{
	return fd_;
}

void Socket::close() {
	if (fd_ >= 0) {
		::close(fd_);
		fd_ = -1;
	}
	is_listening_ = false;
}