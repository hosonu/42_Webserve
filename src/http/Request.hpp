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
#include "../config/Config.hpp"

class Request
{

public:
	Request();
	~Request();
	bool 	requestParse(const std::string &rawRequest);
	bool 	lineParse(const std::string &rawRequest);
	bool 	headerParse(const std::string &rawRequest);
    bool 	bodyParse(const std::string& rawRequest, bool& flag, bool& checkEmptyLine);
	void 	checkValidReqLine();
	bool	checkBodyExist();
	void	makeBody(char *buffer);

    std::string getQuery();
	std::string getMethod();
	std::string getUri();
	std::string getVersion();
	std::map<std::string, std::string> getHeader();
	std::string &getRawHeader();
	const bool	&getCgMode();
	void	setRawHeader(char *buffer);
	void	setCgMode(bool mode);
	void	setBody(char *buffer);

private:
    std::string keyword;
	std::string method;
	std::string uri;
	std::string version;
	std::map<std::string, std::string> headers;
	std::string body;

	bool cgMode;
	bool setLine;
	std::string rawHeader;
};

void	print_line(Request& test);

#endif