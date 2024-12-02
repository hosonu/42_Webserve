#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <cstdlib>
#include <algorithm>

class Location
{
	private:
		std::string path;
		std::string root;
		std::vector<std::string> allowMethods;
		bool autoindex;
		std::string indexFile;
		std::string	return_path;
		bool	isCGI;
	public:
		void	setPath(const std::string& path);
		void	setRoot(const std::string& r);
		void	setAllowMethods(const std::string& methods);
		void	setAutoindex(const std::string& ai);
		void	setIndexFile(const std::string& index);
		void	setReturnPath(const std::string& r_path);
		void	setCGI(const std::string& flags);

		const std::string& getPath() const;
		const std::string& getRoot() const;
		const std::vector<std::string>& getAllowMethods() const;
		bool isAutoindex() const;
		const std::string& getIndexFile() const;
		bool	flagCGI() const;
};

#endif
