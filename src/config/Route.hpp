#ifndef ROUTE_HPP
#define ROUTE_HPP

#include <string>
#include <vector>

struct Route
{
	//we have to add more variaus values
	std::string path = "/";
	std::string root = "";
	std::vector<std::string> allowMethods;
	bool autoindex = false;
	std::string indexFile = "index.html";
};

#endif
