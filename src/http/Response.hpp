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
#include <cstdio>
#include "../handler/CGIHandler.hpp"

class Request;

#define NORMAL_FILE 1
#define DIRECT 2
#define ELSE 0

class Response
{
private:
    int statCode;
    bool cgiFlag;
    std::string request_line;
    std::string situation;
    std::string header;
    std::string body;
    std::string cgiBody;
    std::string truePath;
    std::string cgiPath;
    Location serverLocation;
public:
    Response();
    ~Response();

    std::string createMessage(Request &req, ServerConfig& conf);
    std::string createTruePath(ServerConfig& conf, const std::string &uri);
    std::string createErrorPath(ServerConfig& conf);
    std::string createErrorPage(int statusCode, const std::string& statusMessage);
    std::string getContentType(const std::string& filePath);
    std::string getContentLength();

    bool checkMatching(const std::string &locPath, const std::string &uri);
    int checkFileType(std::string path);
    bool checkErroPath(ServerConfig& conf);
    std::string getDate();
    std::string getServer();
    std::string getConnection();
    std::string getRequestLine();
    
    void getStatusCode();
    void    getBodyGet(std::string path, ServerConfig& conf, std::string uri);
    void    getBodyPost(Request& req, ServerConfig& conf);
    void    getBodyDel(ServerConfig& conf);
    void    wirteMessage(int socket);
    void    setStatusCode(int parseNum, int confNum);
    bool    serachCgiPath(ServerConfig& conf);
	void setCGIBody(std::string body);

    void readErrorFile(std::ifstream& error);
    std::string addIndexFile(std::string res);
    void set_headers(Request &req);
};

std::string generateRandomFileName(std::string dir) ;
std::string generateDirectoryListing(const std::string& path, const std::vector<std::string>& files);
std::vector<std::string> getContents(const std::string& path, const std::string& uri);

#endif