#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include "Route.hpp"
#include <string>
#include <map>

class ServerConfig {
	public:
		ServerConfig();
		~ServerConfig();



		void setHost(const std::string& host);
		void setListenPort(int port);
		std::string getHost() const;
		int	getListenPort() const;

	private:
		//server definition data & groval settigns data
		int	listenPort;
		std::string host;
		std::string serverName;
		std::map<int, std::string> errorPages;
		std::string maxBodySize;//it's okay to get num(size_t or int)

		/*we should make route class or route data*/
		//route settings data
		Route routeData;
};

#endif
