#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <vector>
#include <sys/epoll.h>
#include <exception>
#include <iostream>

#define MAX_BUFEER 1024
// #define MAX_EVENTS 10;

enum class ClientMode {
    READING,
    WRITING,
    CLOSING
};

class Client { 
    private:
        int client_fd;
        ClientMode mode;
        std::vector<char>   read_buffer;
        std::vector<char>   write_buffer;
    
    public:
        Client(int fd, int epoll_fd);
        // void    handleClientEvent(uint32_t events);

        void    setMode(ClientMode mode);
        int getClientFd() const;
};

#endif
