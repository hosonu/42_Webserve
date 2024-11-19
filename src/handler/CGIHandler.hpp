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

class CGIHandler
{
private:
    std::map<std::string, std::string> env;
    std::string cgiPath;
    std::string filePath;
    request* req;
    char **envp;
    void InitCGIPath();
public:
    CGIHandler();
    ~CGIHandler();
    void getEnvAsChar();
    void getPathInfo();
    void CGIExecute();
};


#endif