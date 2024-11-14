#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include "Route.hpp"
#include <string>
#include <map>

struct ServerConfig
{
	bool is_default;
	int	listenPort;
	std::string host;
	std::string serverName;
	std::map<int, std::string> errorPages;
	std::string maxBodySize;//it's okay to get num(size_t or int)
	std::vector<Route> LocationData;
};


#endif
