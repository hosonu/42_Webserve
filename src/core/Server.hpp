#ifndef SERVER_HPP
#define SERVER_HPP

#include "Socket.hpp"
#include <sys/epoll.h>
#include <vector>

#define MAX_EVENTS 10

/*main class of this server*/
class Server {
	public:
		Server(const std::string& host, int port);
		~Server();
	//manage event loop using epoll
	//manage socket and accept connection of cliant
	//control non-blocking I/O process
		void	run();
	private:
		Socket socket_;
		int	epoll_fd_;
		std::vector<struct epoll_event> events_;

		void	handleClient(int client_fd);
};

#endif
