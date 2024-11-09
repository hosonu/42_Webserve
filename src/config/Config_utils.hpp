#ifndef CONFIG_UTILS_HPP
#define CONFIG_UTILS_HPP

#include "Config.hpp"

void	getListenDirective(std::string &line, ServerConfig &currentServer);
void	getErrorPage(std::string &line, ServerConfig &currentServer);
// void	getRouteData(std::string &line, ServerConfig &currentServer);
Route	getRouteData(std::string &line, std::stringstream &streamConf);
bool	checkValidDirective(std::stringstream &file);
bool	isValidIpAddress(const std::string &ip);
bool	isValidErrorPages(const std::map<int, std::string>& errorPages);
bool	isValidMaxBodySize(const std::string &maxBodySize);
// bool	isValidRouteData(const Route &routeData); 
bool	isValidRouteData(const std::vector<Route> &locationData);


#endif
