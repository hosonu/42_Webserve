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
#include <string.h>

int main()
{
        char * envp[] = {
            "REQUEST_METHOD=POST",
            "SCRIPT_NAME=/cgi/bin/hello.py",
            "QUERY_STRING=",
            "CONTENT_TYPE=application/x-www-form-urlencoded",
            "CONTENT_LENGTH=13", 
            "SERVER_NAME=example.com",
            "SERVER_PORT=80",
            "SERVER_PROTOCOL=HTTP/1.1",
            NULL
        };
    char *argv[] = {
        "python3",
        "cgi/bin/hello.py",
        NULL
    };
    execve("/usr/bin/python3", argv, envp);
}