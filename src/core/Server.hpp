#ifndef SERVER_HPP
#define SERVER_HPP

#include "Socket.hpp"
#include "../config/Config.hpp"
#include <sys/epoll.h>
#include <vector>

#define MAX_EVENTS 10

/*main class of this server*/
class Server {
	public:
		// Server(const std::vector<ServerConfig> &configs);
		Server(Config &configs);
		~Server();
	//manage event loop using epoll
	//manage socket and accept connection of cliant
	//control non-blocking I/O process
		void	run();

		void	setServer();
	private:
		std::vector<ServerConfig> configData;
		std::vector<Socket> socket_;
		int	epoll_fd_;
		// std::vector<ServerInstance> server_instances_;
		// std::map<int, const ServerConfig*> client_configs_; // Maps client FDs to their configurations
		std::vector<struct epoll_event> events_;

		//initialize each server instance
		void	acceptNewConnection(Socket& listen_socket);
		void	setNonBlocking_cs(int fd);
		//handling client connection
		void	handleClient(int client_fd);
};

#endif
