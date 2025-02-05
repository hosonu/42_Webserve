#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <iostream>
#include <cstring>
#include <sstream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <cerrno>


class Socket {
	public:
		Socket();
		Socket(const std::string& host, int port);
		Socket(const Socket &src);
		~Socket();

		bool	bind();
		bool	listen(int backlog = 10);
		int		accept();
		bool	setNonBlocking(int fd);

		int		getFd() const;
		void    close();

	private:
		int	fd_;
		std::string host_;
		int	port_;
		struct sockaddr_in	address_;
		bool	is_listening_;
};

unsigned long ipToLong(const std::string& ip);

#endif
