#ifndef HTTP_PARSE
#define HTTP_PARSE

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
#include <cctype>

class Request;

class HttpParse
{
	public:
		HttpParse();
		~HttpParse();
		//status-line
		void checkReqLine(const std::string reqLine, std::string method, std::string uri, std::string ver);
		bool checkSp(const std::string reqLine);
		bool checkMethod(std::string method);
		bool checkUri(std::string &uri);
		bool checkVersion(std::string ver);
		//header
		bool checkStructure(std::string headLine, std::map<std::string, std::string>& headers, std::string& keyword);

		//body
		void unChunckedBody(std::string body, std::string& unChunk);
		
		//Getter
		void setHeaderStatus(int set);
		void setTotalStatus();
		int getHeaderStatus();
		int getStartStatus();
		int getTotalStatus();
	private:
		int startStatus;
		int headerStatus;
		int totalStatus;
		std::string method;
		std::string requestTraget;
		std::string httpVersion;
};

bool isValidHeaderFieldName(const std::string& fieldName);
bool isValidHeaderFieldValue(const std::string& fieldValue);
std::string decodeURI(const std::string& encodedURI);
std::string replaceSpecialChars(const std::string& decodedURI);

#endif