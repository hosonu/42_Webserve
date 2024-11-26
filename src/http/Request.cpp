#include "Request.hpp"

Request::Request() : cgMode(false)
{
}

Request::~Request()
{
}

bool Request::requestParse(const std::string &rawRequest)
{
    std::istringstream stream(rawRequest);
    std::string line;
    bool flag = true;
    if (!std::getline(stream, line) || !lineParse(line))
        return false;
    while (std::getline(stream, line))
    {
        //std::cout << line << std::endl;
        if (!headerParse(line) && flag)
            return false;
        else if (line == "\r")
        {
            if (line == "")
                break;
            this->body += line;
        }
    }
    //this->parse.unChunckedBody(this->body);
    this->parse.setTotalStatus();
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
    this->parse.checkReqLine(lineRequest, this->method, this->uri, this->version);
	return true;
}

bool Request::headerParse(const std::string &headerRequest)
{
    this->parse.checkStructure(headerRequest, this->headers, this->keyword);
    std::string check = this->headers["Host"];
    if (check.empty())
    {
        this->parse.setHeaderStatus(400);
    }
    return true;
}

bool	Request::checkBodyExist() {
	std::map<std::string, std::string>::iterator it = headers.find("Content-Length");
	if (it != headers.end()) {
		this->content_length = it->second;
		return true;
	} else {
		return false;
	}
}

bool	Request::isBodyComplete() const {
	size_t expected_length = static_cast<size_t>(std::strtol(this->content_length.c_str(), NULL, 10));
	return this->body.length() >= expected_length;
}

void	Request::appendBody(char *buffer) {
	std::string add_body = buffer;
	this->body += buffer;
}

std::string& Request::getBody() {
	return this->body;
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

HttpParse Request::getPrse()
{
    return this->parse;
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

void	Request::setBody(std::string body) {
	this->body = body;
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


    std::cout << "is_default: " <<test.getDefault() << std::endl;
    std::cout << "listenPort: "<<test.getListenPort() << std::endl;
    std::cout << "host: "<<test.getListenHost() << std::endl;
    std::cout << "serverName:" <<test.getServerName() << std::endl;
    std::cout << "maxBodySize:" <<test.getMaxBodySize() << std::endl;

	
    for (std::vector<Location>::const_iterator it = test.getLocations().begin(); it != test.getLocations().end(); ++it) {
        std::cout << "Location.path: " << it->getPath() << std::endl;
        std::cout << "Location.root: " << it->getRoot() << std::endl;
        for (std::vector<std::string>::const_iterator methodIt = it->getAllowMethods().begin();
             methodIt != it->getAllowMethods().end(); ++methodIt) {
            std::cout << *methodIt << " ";
        }
        std::cout << "autoindex: " << it->isAutoindex() << std::endl;
        std::cout << "indexFile: " << it->getIndexFile() << std::endl;
    }

}