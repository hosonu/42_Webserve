#include "Request.hpp"

Request::Request() : cgMode(false), setLine(false)
{
}

Request::~Request()
{
}

bool Request::requestParse(const std::string &rawRequest, ServerConfig conf)
{
    std::istringstream stream(rawRequest);
    std::string line;
    bool flag = true;
    print_conf(conf);
    if (!std::getline(stream, line) || !lineParse(line))
        return false;
    while (std::getline(stream, line) && line != "\r")
    {
        if (!headerParse(line) && flag)
            return false;
        if (line == "\r\n")
        {
            continue;
            if (line == "")
                break;
            this->body += line;
        }
    }
    return true;
}


bool Request::lineParse(const std::string& lineRequest)
{
	std::istringstream stream(lineRequest);
	if (!std::getline(stream, this->method, ' '))
		return false;
	if (!std::getline(stream, this->uri, ' '))
		return false;
	if (!std::getline(stream, this->version))
		return false;
    //if (!checkValidReqLine())
    //    return false;
	return true;
}

//bool Request::checkValidReqLine()
//{
//    bool flag;
//    flag = checkValidMethod();
//    flag = checkValidUri();
//    flag = checkValidVersion();
//    return flag;
//}

//bool Request::checkValidMethod()
//{
    
//}


bool Request::headerParse(const std::string &headerRequest)
{
    size_t pos = headerRequest.find(":");
    std::string key;
    std::string val;
    if (pos == std::string::npos)
        return false;
    key = headerRequest.substr(0, pos);
    val = headerRequest.substr(pos + 1);
    val.erase(0, val.find_first_not_of(" \t"));
    val.erase(val.find_last_not_of(" \t") + 1);
    this->headers[key] = val;
    if (key == "Content-Type:")
        this->keyword = val.substr(val.find("="));    
    return true;
}

bool	Request::checkBodyExist() {
	std::map<std::string, std::string>::iterator it = headers.find("Content-Length");

	if (it != headers.end()) {
		return true;
	} else {
		return false;
	}
}

void	Request::makeBody(char *buffer) {
	std::string add_body = buffer;
	this->body += buffer;
}

std::string Request::getMethod()
{
	return this->method;
}

std::string Request::getUri()
{
	return this->uri;
}
std::string Request::getVersion()
{
	return this->version;
}

std::map<std::string, std::string> Request::getHeader()
{
	return this->headers;
}

std::string	&Request::getRawHeader() {
	return this->rawHeader;
}

const bool	&Request::getCgMode() {
	return this->cgMode;
}

void Request::setRawHeader(char *buffer) {
	this->rawHeader = buffer;
}

void	Request::setCgMode(bool mode) {
	this->cgMode = mode;
}

std::string Request::getQuery()
{
    if (this->uri.find("?") != std::string::npos)
    {
        return this->uri.substr(this->uri.find("?"));
    }
    else
        return "";
}

//test code
void    print_line(Request& test)
{
    std::map<std::string, std::string> t_map = test.getHeader();
    std::cout << "method: " << test.getMethod() << std::endl;
    std::cout << "uri: " << test.getUri() << std::endl;
    std::cout << "version: " << test.getVersion() << std::endl;
    for (std::map<std::string, std::string>::iterator ite = t_map.begin(); ite != t_map.end(); ite++) {
        std::cout << "Key = " << ite->first << ", Value = " << ite->second << std::endl;
    }
}

void    print_conf(ServerConfig test)
{
    std::cout << "is_default: " <<test.is_default << std::endl;
    std::cout << "listenPort: "<<test.listenPort << std::endl;
    std::cout << "host: "<<test.host << std::endl;
    std::cout << "serverName:" <<test.serverName << std::endl;
    for (std::map<int, std::string>::iterator it = test.errorPages.begin(); it != test.errorPages.end(); ++it) {
        std::cout << "Key: " << it->first << ", Value: " << it->second << std::endl;
    }
    for (std::vector<Route>::iterator it = test.LocationData.begin(); it != test.LocationData.end(); ++it) {
        std::cout << "Route.path: " << it->path << std::endl;
        std::cout << "Route.root" << it->root << std::endl;
        for (std::vector<std::string>::const_iterator methodIt = it->allowMethods.begin();
             methodIt != it->allowMethods.end(); ++methodIt) {
            std::cout << *methodIt << " ";
        }
        std::cout << "autoindex: " << it->autoindex << std::endl;
        std::cout << "indexFile: " << it->indexFile << std::endl;
    }
}