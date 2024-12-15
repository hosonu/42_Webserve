#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <vector>
#include <sys/epoll.h>
#include <exception>
#include <iostream>
#include <cstdlib>

#include "Server.hpp"
#include "../http/Request.hpp"
#include "../http/Response.hpp"
#include "../handler/CGIHandler.hpp"
#include "../handler/CGIHandler.hpp"
#define MAX_BUFEER 1024

enum ClientMode
{
	HEADER_READING,
	BODY_READING,
	WAITING,
	CGI_READING,
	WRITING,
	CLOSING
};

class Server;

class Client
{
	public:
		Client(int fd, int epoll_fd);
		void	bindToConfig(std::vector<ServerConfig> &configData);
		void 	parseRequestHeader(std::vector<ServerConfig> &configData);
		void	parseRequestBody();
		void	makeResponse();
		void	methodProc();
		void 	updateEpollEvent();
		int					getClientFd() const;
		const ClientMode	&getClientMode() const;
		const ServerConfig	&getConfigDatum() const;
		int					getCGIfd() const;
		void				setMode(ClientMode mode);

		void updateActivity() {
        	last_activity = time(NULL);
    	}

		bool isTimedOut(time_t current_time, time_t timeout_seconds) const {
			return (current_time - last_activity) > timeout_seconds;
		}

		void readCGI();
		Client& operator=(const Client& other);
		Client(const Client& other);
		void end_timeoutCGI();
	private:
		int client_fd;
		int epfd;
		int cgi_fd;
		pid_t child_pid;
		ClientMode mode;

		time_t	last_activity;

		std::vector<char> read_buffer;
		std::vector<char> write_buffer;
		std::string rawReq;
		Request req;
		Response msg;
		CGIHandler cgi;
		ServerConfig	configDatum;
};

#endif
