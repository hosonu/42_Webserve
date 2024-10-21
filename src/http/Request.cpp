#include "Request.hpp"

request::request()
{

}

request::~request()
{
    
}

bool request::requestParse(const std::string& rawRequest)
{
    std::istringstream stream(rawRequest);
    std::string line;

    if (!std::getline(stream, line) || !lineParse(line))
        return false;
    
    while (std::getline(stream, line) && line != "\r")
    {
        if (!headerParse(line))
            return false;
    }
    return true;
}

bool request::lineParse(const std::string& lineRequest)
{
    std::istringstream stream(lineRequest);
    if (!std::getline(stream, this->method, ' '))
        return false;
    if (!std::getline(stream, this->uri, ' '))
        return false;
    if (!std::getline(stream, this->version))
        return false;
    return true;
}

bool request::headerParse(const std::string& headerRequest)
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
    return true;
}

std::string request::getMethod()
{
    return this->method;
}

std::string request::getUri()
{
    return this->uri;
}
std::string request::getVersion()
{
    return this->version;
}

std::map<std::string, std::string> request::getHeader()
{
    return this->headers;
}

void    print_line(request& test)
{
    std::map<std::string, std::string> t_map = test.getHeader();
    std::cout << "method: " << test.getMethod() << std::endl;
    std::cout << "uri: " << test.getUri() << std::endl;
    std::cout << "version: " << test.getVersion() << std::endl;
    for (auto ite = t_map.begin(); ite != t_map.end(); ite++) {
        std::cout << "Key = " << ite->first << ", Value = " << ite->second << std::endl;
    }
}