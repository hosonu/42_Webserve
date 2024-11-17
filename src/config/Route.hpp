#ifndef ROUTE_HPP
#define ROUTE_HPP

#include <string>
#include <vector>

struct Route
{
	std::string path;
	std::string root;
	std::vector<std::string> allowMethods;
	bool autoindex;
	std::string indexFile;
};

#endif
