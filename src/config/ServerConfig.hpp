#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include "Location.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstring>
#include <map>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>

template <typename T>
std::string customToString(const T& value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

struct LocationCompare {
	int countSlashes(const std::string& path) const {
		int count = 0;
		for (std::string::size_type i = 0; i < path.length(); ++i) {
			if (path[i] == '/') {
				++count;
			}
		}
		return count;
	}

	bool operator()(const Location& a, const Location& b) const {
		int slashesA = countSlashes(a.getPath());
		int slashesB = countSlashes(b.getPath());
		
		if (slashesA != slashesB) {
			return slashesA < slashesB;
		}
		return a.getPath().length() < b.getPath().length();
	}
};

class ServerConfig {
private:
	bool	is_default;
	int		listenPort;
    std::string host;
    std::string serverName;
	std::map<int, std::string> errorPages;
	std::string maxBodySize;
	std::vector<Location> locations;

public:
	ServerConfig();
	ServerConfig(const ServerConfig& other);
	ServerConfig& operator=(const ServerConfig& other);
	void	setDefault(bool flag);
    void	setListen(std::string& listen);
    void	setServerName(const std::string& name);
    void	setErrorPage(const std::string& page);
	void	setErrorPage(const std::vector<std::string>& tokens, size_t& i);
	void	setClientMaxBodySize(const std::string& size);
    void	addLocation(const std::vector<std::string>& tokens, size_t& index);
	void	sortLocations();

    bool getDefault() const;
    std::string getListenHost() const;
	int	getListenPort() const;
    std::string getServerName() const;
    const std::map<int, std::string>& getErrorPages() const;
    std::string getMaxBodySize() const;
    const std::vector<Location>& getLocations() const;
};

std::string getLocalhostIpv4();
std::string convertIpToString(uint32_t ipAddress);


#endif
