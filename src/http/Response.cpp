#include "Response.hpp"

Response::Response()
{}

Response::~Response()
{}

void Response::createMessage(int stat, const std::string& path)
{
    this->statCode = stat;
    if (stat == 200)
        this->situation = "OK";
    else
        this->situation = "NG";
    this->getBody(path);
    this->header += this->getContentType(path) + "\n";
    this->header += this->getContentLength();
    this->header += this->getDate();
    this->header += this->getServer();
    this->header += this->getConnection();

}

void Response::getBody(const std::string& path)
{
    //TODO: autoindex
    std::ifstream stream(path);
    if (!stream.is_open())
    {
        //TOD0: error Proc
        return ;
    }
    while (std::getline(stream, this->body))
    {}
    
}

std::string Response::getContentType(const std::string& filePath)
{
    std::string extension = filePath.substr(filePath.find_last_of(".") + 1);


    static const std::map<std::string, std::string> mimeTypes = {
        {"html", "text/html"},
        {"htm", "text/html"},
        {"css", "text/css"},
        {"js", "application/javascript"},
        {"json", "application/json"},
        {"png", "image/png"},
        {"jpg", "image/jpeg"},
        {"jpeg", "image/jpeg"},
        {"gif", "image/gif"},
        {"svg", "image/svg+xml"},
        {"xml", "application/xml"},
        {"pdf", "application/pdf"},
        {"txt", "text/plain"}
        // 他の拡張子とMIMEタイプのペアを追加
    };

    std::map<std::string, std::string>::const_iterator it = mimeTypes.find(extension);
    if (it != mimeTypes.end()) {
        return it->second;
    }

    return "application/octet-stream";  // デフォルトのMIMEタイプ
}

std::string Response::getContentLength()
{
    std::ostringstream oss;
    oss << this->body.length();
    return oss.str();
}

std::string Response::getDate()
{
    char buffer[100];
    time_t now = time(0);
    struct tm* gmt = gmtime(&now);
    
    strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", gmt);
    return "Date: " + std::string(buffer);
}

std::string Response::getServer()
{
    return "Server: Apache/2.4.41";
}

std::string Response::getConnection()
{
    return "Connection: Alive";
}

void Response::wirteMessage(int socket)
{
    int headerLen = this->header.length();
    int bodyLen = this->body.length();
    //TODO: Error handling
    write(socket, this->header.c_str(), headerLen);
    write(socket, "\r\n", 3);
    write(socket, this->body.c_str(), bodyLen);
}