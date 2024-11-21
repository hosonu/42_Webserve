#ifndef HSERVERCONFIG_HPP
#define HSERVERCONFIG_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>

struct Route
{
	std::string path;
	std::string root;
	std::vector<std::string> allowMethods;
	bool autoindex;
	std::string indexFile;
};

class HServerConfig {
private:
	bool	is_default;
	int		listenPort;
    std::string host;
    std::string serverName;
	std::map<int, std::string> errorPages;
	std::string maxBodySize;

	std::vector<Route> locations;
    //std::vector<std::pair<std::string, std::string>> locations;

public:
    void setListen(std::string& listen);
    void setServerName(const std::string& name);
    void setRootPath(const std::string& path);
    void setErrorPage(const std::string& page);
    void addLocation(const std::vector<std::string>& tokens, size_t& index);

    std::string getListen() const;
    std::string getServerName() const;
};

#endif
