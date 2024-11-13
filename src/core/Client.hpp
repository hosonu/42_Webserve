#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <vector>
#include <sys/epoll.h>
#include <exception>
#include <iostream>

#include "Server.hpp"
#include "../http/Request.hpp"

#define MAX_BUFEER 1024

enum ClientMode {
    HEADER_READING,
    BODY_READING,
    WRITING,
    CLOSING
};



class Server;

class Client { 
    private:
        int client_fd;
        ClientMode mode;
        std::vector<char>   read_buffer;
        std::vector<char>   write_buffer;
        std::vector<ServerConfig> configData;
        std::string rawReq;
        request req;
    
    public:
        Client(int fd, int epoll_fd);

        void    setMode(ClientMode mode);
        int     getClientFd() const;
        const ClientMode& getClientMode() const;

        void    parseRequestHeader();
        void    makeResponse();
};

#endif
