#ifndef CONFIG_UTILS_HPP
#define CONFIG_UTILS_HPP

#include "Config.hpp"

void	getListenDirective(std::string &line, ServerConfig &currentServer);
void	getErrorPage(std::string &line, ServerConfig &currentServer);
void	getRouteData(std::string &line, ServerConfig &currentServer);
bool	checkBraceFlags(std::stringstream &file);
bool	isValidIpAddress(const std::string &ip);
bool isValidErrorPages(const std::map<int, std::string>& errorPages);
bool	isValidMaxBodySize(const std::string &maxBodySize);

#endif
