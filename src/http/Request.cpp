#include "Request.hpp"

Request::Request() : cgMode(false)
{
    this->body = "";
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
        if (!headerParse(line) && flag)
            return false;
    }
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
    this->parse.checkStructure(headerRequest, this->headers);
    std::string check = this->headers["Host"];
    if (check.empty())
    {
        this->parse.setHeaderStatus(400);
        return true;
    }
    check.erase(check.length() - 1);
    if (check.find(":") == std::string::npos)
    {
        this->parse.setHeaderStatus(400);
        return true;
    }
    if (check.empty() || !isValidPort(check.substr(check.find(":") + 1)) || !isValidHostName(check.substr(0, check.find(":"))))
    {
        if (check.substr(0, check.find(":")) != "localhost")
        {
            this->parse.setHeaderStatus(400);
        }
    }
    return true;
}

bool isValidHostName(const std::string& host) {
    size_t start = 0, end;
    std::string part;
    while ((end = host.find('.', start)) != std::string::npos) 
    {
        part = host.substr(start, end - start);
        if (part.empty() || part.length() > 63)
        {
            return false;
        }
        for (size_t i = 0; i < part.length(); ++i) {
            if (!isalnum(part[i]) && part[i] != '-') 
            {
                return false;
            }
        }
        if (part[0] == '-' || part[part.length() - 1] == '-') 
        {   
            return false;
        }
        start = end + 1;
    }
    if (part.empty() || part.length() > 63) 
    {   
        return false;
    }
    for (size_t i = 0; i < part.length(); ++i) {
        if (!isalnum(part[i]) && part[i] != '-') 
        {   
            return false;
        }
    }
    if (part[0] == '-' || part[part.length() - 1] == '-') 
    {
        return false;
    }
    return true;
}

bool isValidPort(const std::string& port) {
    for (size_t i = 0; i < port.length(); ++i) 
    {
        if (!isdigit(port[i]))
        {
            return false;
        }
    }
    return true;
}

void Request::bodyParse()
{
    if(this->headers["Transfer-Encoding"] == "chunked")
        this->parse.unChunckedBody(this->body, this->unChunked);
    else
        this->unChunked = this->body;
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

void	Request::appendBody(const std::string &buffer) {
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

void Request::setRawHeader(const std::string &buffer) {
	this->rawHeader += buffer;
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

//debug functions
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
