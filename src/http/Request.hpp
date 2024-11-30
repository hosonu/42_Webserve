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
#include "../config/Config.hpp"
#include "HttpParse.hpp"

class Response;

class Request
{

public:
	Request();
	~Request();
	bool 	requestParse(const std::string &rawRequest);
	bool 	lineParse(const std::string &rawRequest);
	bool 	headerParse(const std::string &rawRequest);
	bool	checkBodyExist();
	bool	isBodyComplete() const;
	void	appendBody(char *buffer);
	void	bodyParse();
	std::string &getBody();
    std::string getQuery();
	std::string getMethod();
	std::string getUri();
	std::string getVersion();
	std::map<std::string, std::string> getHeader();
	std::string &getRawHeader();
	const bool	&getCgMode();
	void	setRawHeader(const std::string &buffer);
	void	setCgMode(bool mode);
	void	setBody(std::string body);
	HttpParse getPrse();
private:
    std::string keyword;
	std::string method;
	std::string uri;
	std::string version;
	std::map<std::string, std::string> headers;
	std::string body;
	std::string unChunked;
	HttpParse parse;
	std::string content_length;

	bool cgMode;
	std::string rawHeader;
};

void	print_line(Request& test);
void    print_conf(ServerConfig test);

#endif