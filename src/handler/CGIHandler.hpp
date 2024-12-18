#ifndef CGI_HANDLER

#define CGI_HANDLER

#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <cerrno>
#include <map>
#include <sstream>
#include <string>
#include <fstream>
#include "../http/Request.hpp"
#include <string.h>

#include <sys/epoll.h>

class Client;

#define BUFFER_CGI 1024

class CGIHandler
{
private:
    std::map<std::string, std::string> env;
    std::string cgiPath;
    std::string filePath;
    std::string res;
    Request req;
    char **envp;
	std::string newBody;
    int epfd_;
    pid_t child_pid;

    void getEnvAsChar();
    void getPathInfo();
    void InitCGIPath();
public:
    CGIHandler();
    CGIHandler(Request req,int epfd);
	CGIHandler(const CGIHandler& other);
	CGIHandler& operator=(const CGIHandler& other);
    ~CGIHandler();
    int CGIExecute();
    bool    addContentLength(const std::string& httpResponse);
	std::string	getCGIBody();
	void appendCGIBody(const std::string &buffer);
    pid_t getChildPid();
    std::string getRes();
};


#endif