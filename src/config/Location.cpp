#include "Location.hpp"

Location::Location()
: path(""), root(""), allowMethods(), autoindex(false), indexFile(""), return_path(""), isCGI(false) {}

void Location::setPath(const std::string& path) {
	if (path[0] != '/') {
			throw std::invalid_argument("[emerg] unexpected path in location{} : " + path);
	}
    this->path = path;
}

void Location::setRoot(const std::string& r) {
	if (r[0] != '/') {
		throw std::invalid_argument("[emerg] unexpected root in location{} : " + r);
	}
    this->root = r;
}

void Location::setAllowMethods(const std::string& methods) {
	if (methods != "GET" && methods != "POST" && methods != "DELETE") {
		throw std::invalid_argument("[emerg] unexpected methods in location{} : " + methods);
	}
	this->allowMethods.push_back(methods);
}

void Location::setAutoindex(const std::string& ai) {
	if (ai == "on") {
    	this->autoindex = true;
	} else if (ai == "off") {
    	this->autoindex = false;
	} else 
		throw std::invalid_argument("[emerg] unexpected autoindex in location{} : " + ai);
}

void Location::setIndexFile(const std::string& index) {
	if (index.length() < 5 || 
		index.substr(index.length() - 5) != ".html") {
			throw std::invalid_argument("[emerg] unexpected indexFile in location{} : " + index);
	}
    this->indexFile = index;
}

void	Location::setReturnPath(const std::string& r_path) {
	if (r_path[0] != '/') {
		throw std::invalid_argument("[emerg] unexpected root in location{} : " + r_path);
	}
    this->return_path = r_path;
}

void	Location::setCGI(const std::string& flag) {
	if (flag == "on") {
    	this->autoindex = true;
	} else if (flag == "off") {
    	this->autoindex = false;
	} else 
		throw std::invalid_argument("[emerg] unexpected autoindex in location{} : " + flag);
}

const std::string& Location::getPath() const {
    return this->path;
}

const std::string& Location::getRoot() const {
    return this->root;
}

const std::vector<std::string>& Location::getAllowMethods() const {
    return this->allowMethods;
}

bool Location::isAutoindex() const {
    return this->autoindex;
}

const std::string& Location::getIndexFile() const {
    return this->indexFile;
}

bool	Location::flagCGI() const {
	return this->isCGI;
}
const std::string& Location::getReturnPath() const {
    return this->return_path;
}
