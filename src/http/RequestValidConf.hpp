#ifndef REQUSET_VALID_CONF
#define REQUSET_VALID_CONF

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

class RequestValidConf
{
	private:
		Request req;
		Location loc;
		int	stat;
	public:
		RequestValidConf();
		RequestValidConf(Request& req, Location& loc);
		~RequestValidConf();
		bool checkAllow(std::string method, std::vector<std::string>& allows);
		void validReqLine();
		int getStat();
};

#endif