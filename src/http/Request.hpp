#ifndef REQUEST_HPP
#define REQUEST_HPP

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
#include "Response.hpp"


class request
{
    public:
        request();
        ~request();
        bool requestParse(const std::string& rawRequest);
        bool lineParse(const std::string& rawRequest);
        bool headerParse(const std::string& rawRequest);
        // bool bodyParse();
        std::string getMethod();
        std::string getUri();
        std::string getVersion();
        std::map<std::string, std::string> getHeader();
        void methodProc(int clinet_fd);

        void    setRawHeader(char* buffer) {
            rawHeader = buffer;
        }

        std::string& getRawHeader() {
            return rawHeader;
        }
    private:
        std::string method;
        std::string uri;
        std::string version;
        std::map<std::string, std::string> headers;
        std::string body;

        std::string rawHeader;
};

void    print_line(request& test);

#endif