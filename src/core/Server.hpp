#ifndef SERVER_HPP
#define SERVER_HPP

#include "Socket.hpp"
#include "../config/Config.hpp"
#include "../http/Request.hpp"
#include "Client.hpp"
#include <sys/epoll.h>
#include <vector>
#include <list>

#define MAX_EVENTS 1024

const int EPOLL_TIMEOUT_MS = 1000;
const time_t CLIENT_TIMEOUT_SEC = 10;

class Client;
class Server {
	public:
		Server(Config &configs);
		~Server();

		void	run();
		void	setServer();
		void	closeServer();
	private:
		void	acceptNewConnection(Socket& listen_socket);
		void	HandleRequest(Client &client);
		void	HandleResponse(Client &client);
		void	removeClient(int client_fd);

		struct epoll_event events_[MAX_EVENTS];
		std::vector<ServerConfig> configData;
		std::vector<Socket> socket_;
		std::list<Client> client_;
		int	epoll_fd_;
};

#endif
