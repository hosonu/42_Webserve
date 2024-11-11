#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include "Route.hpp"
#include <string>
#include <map>

struct ServerConfig
{
	//server definition data & groval settigns data
	bool is_default;
	int	listenPort;
	std::string host;
	std::string serverName;
	std::map<int, std::string> errorPages;
	std::string maxBodySize;//it's okay to get num(size_t or int)

	/*we should make route class or route data*/
	//route settings data
	std::vector<Route> LocationData;
};


#endif
