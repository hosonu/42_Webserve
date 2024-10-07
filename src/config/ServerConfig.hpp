#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include <cstring>
#include <map>

class ServerConfig {
	public:
		
	private:
		//server definition data & groval settigns data
		int	listenPort;
		std::string host;
		std::string serverName;
		std::map<int, std::string> errorPages;
		std::string maxBodySize;//it's okay to get num(size_t or int)


		/*we should make route class or route data*/
		//route settings data
		std::string rootPath;
		//etc

};

#endif
