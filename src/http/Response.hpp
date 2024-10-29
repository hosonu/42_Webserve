#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <cerrno>
#include <map>
#include <sstream>
#include <string>
#include <fstream>
#include <ctime>
#include "Request.hpp"

class Response
{
private:
    int statCode;
    std::string situation;
    std::string header;
    std::string body;
public:
    Response();
    ~Response();
    void createMessage(int stat, const std::string& path);
    std::string getContentType(const std::string& filePath);
    std::string getContentLength();
    std::string getDate();
    std::string getServer();
    std::string getConnection();
    void    getBody(const std::string& path);
    void    wirteMessage(int socket);
};

#endif