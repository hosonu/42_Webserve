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
#include "../config/Config.hpp"
#include "../config/ServerConfig.hpp"
#include "../config/Location.hpp"
#include <dirent.h>
#include <vector>
#include <sys/stat.h>
#include <string>
#include <cerrno>
#include <cstring>
#include "RequestValidConf.hpp"

class Request;

#define NOMAL_FILE 1
#define DIRECT 2
#define ELSE 0

class Response
{
private:
    int statCode;
    std::string request_line;
    std::string situation;
    std::string header;
    std::string body;
    std::string truePath;
public:
    Response();
    ~Response();
    void createMessage(Request &req, ServerConfig& conf);
    std::string createTruePath(ServerConfig& conf);
    std::string createErrorPath(ServerConfig& conf);
    std::string createErrorPage(int statusCode, const std::string& statusMessage);
    std::string getContentType(const std::string& filePath);
    std::string getContentLength();
    void validReqLine(Request &req, ServerConfig& conf);
    bool checkAllow(std::string method, std::vector<std::string>& allows);
    int checkFileType(const std::string& path);
    std::string getDate();
    std::string getServer();
    std::string getConnection();
    std::string getRequestLine();
    void getStatusCode();
    void    getBody(const std::string& path, ServerConfig& conf);
    void    wirteMessage(int socket);
    void    setStatusCode(int parseNum, int confNum);
};

int readfile(std::string path, std::string& body);
std::string generateDirectoryListing(const std::string& path, const std::vector<std::string>& files);
std::vector<std::string> getContents(const std::string& path);

#endif