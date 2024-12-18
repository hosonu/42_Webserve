#include "Socket.hpp"

Socket::Socket() {}

Socket::Socket(const std::string& host, int port) 
: fd_(-1), host_(host), port_(port), is_listening_(false) {
	fd_ = socket(AF_INET, SOCK_STREAM, 0);
	if (fd_ < 0) {
		throw std::runtime_error("Failed to create socket");
	}
	memset(&address_, 0, sizeof(address_));
	address_.sin_family = AF_INET;
	address_.sin_port = htons(port_);
	if (host_ == "0.0.0.0") {
		address_.sin_addr.s_addr = INADDR_ANY;
	} else {
		std::string ip = host_;
		address_.sin_addr.s_addr = htonl(ipToLong(ip));
	}
	return ;
}

Socket::Socket(const Socket &src) {
	fd_ = src.fd_;
	host_ = src.host_;
	port_ = src.port_;
	is_listening_ = src.is_listening_;
	address_ = src.address_;
}

Socket::~Socket() {
}

unsigned long ipToLong(const std::string& ip) {
    std::istringstream iss(ip);
	std::string		token;
    unsigned long result = 0;
	unsigned int	octet;

	while (std::getline(iss, token, '.')) {
		std::istringstream octet_stream(token);
		octet_stream >> octet;

		result = (result << 8) | octet;
	}
    return result;
}

bool	Socket::bind() {
	int opt = 1;

	if (setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		std::cerr << "Failed to setsockopt: " << strerror(errno) << std::endl;
		return false;
	}

	if (::bind(fd_, (struct sockaddr *)&address_, sizeof(address_)) < 0) {
		 std::cerr << "Failed to bind socket to " << host_ << ":" << port_ 
                  << " - Error: " << strerror(errno) 
                  << " (errno: " << errno << ")" << std::endl;
		return false;
	}
	return true;
}

bool	Socket::listen(int backlog) {
	if(::listen(fd_, backlog) < 0) {
		std::cerr << "Failed to listen: " << strerror(errno) << std::endl;
		return false;
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
			std::cerr << "Failed to accept: " << strerror(errno) << std::endl;
			return -1;
		}
		throw std::runtime_error("Failed to accept");
	}
	return client_fd;
}

bool	Socket::setNonBlocking(int fd) {
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1) {
		std::cerr << "Socket is closed or invalid." << std::endl;
		return false;
	}
	if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
		std::cerr << "Failed to set non-blocking mode: " << strerror(errno) << std::endl;
		return false;
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
