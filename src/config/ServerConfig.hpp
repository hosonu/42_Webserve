#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include "Location.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>

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
	void	setDefault(bool flag);
    void	setListen(std::string& listen);
    void	setServerName(const std::string& name);
    void	setErrorPage(const std::string& page);
	void	setErrorPage(const std::vector<std::string>& tokens, size_t& i);
	void	setClientMaxBodySize(const std::string& size);
    void	addLocation(const std::vector<std::string>& tokens, size_t& index);

    bool getDefault() const;
    std::string getListenHost() const;
	int	getListenPort() const;
    std::string getServerName() const;
    const std::map<int, std::string>& getErrorPages() const;
    std::string getMaxBodySize() const;
    const std::vector<Location>& getLocations() const;
};

#endif
