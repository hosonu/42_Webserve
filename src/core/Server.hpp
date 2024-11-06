#ifndef SERVER_HPP
#define SERVER_HPP

#include "Socket.hpp"
#include "../config/Config.hpp"
#include <sys/epoll.h>
#include <vector>
#include "../http/Request.hpp"
#include "Client.hpp"

#define MAX_EVENTS 10

/*main class of this server*/
class Server {
	public:
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
		std::vector<Client> client_;
		int	epoll_fd_;
		struct epoll_event events_[MAX_EVENTS];

		//initialize each server instance
		int	acceptNewConnection(Socket& listen_socket);
		// void	setNonBlocking_cs(int fd);isn't necessary
		//handling client connection
		// void	handleClient(int client_fd);
		void	HandleRequest(Client client);
		void	HandleResponse(Client client);
};

#endif
