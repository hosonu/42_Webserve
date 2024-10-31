#include "Response.hpp"

Response::Response()
{
    this->statCode = 0;
    this->situation = "";
    this->header = "";
    this->body = "";
    this->request_line = "";
}

Response::~Response()
{}

void Response::createMessage(const std::string& path)
{
    this->getBody(path);
    this->getStatusCode();
    this->request_line = this->getRequestLine();
    this->header += this->getContentType(path) + "\r\n";
    this->header += this->getContentLength();
    this->header += this->getDate();
    this->header += this->getServer();
    this->header += this->getConnection();

    // std::cout << "reqline\n" << this->request_line << std::endl;
    // std::cout << "headline\n" << this->header << std::endl;
    // std::cout << "bodyline\n" <<this->body << std::endl;
}

void Response::getBody(const std::string& path)
{
    if (path.at(path.length() - 1) == '/')
    {
        DIR *dir = opendir(path.c_str());
        struct dirent *dent;
        std::string d_name;
        if (!dir)
        {
            //TODO::Error
        }
        while((dent = readdir(dir)) != NULL)
        {
            d_name = dent->d_name;
            if (d_name == "index" || d_name == "index.html")
            {
                this->statCode = readfile(dent->d_name, this->body);
                return ;
            }
        }
        //TODO:: if(AutoIndex) do autoindex;
        this->body = generateDirectoryListing(path, getContents(path));
        this->statCode = 200;
    } 
    else
    {
        this->statCode = readfile(path, this->body);
    }
    
}

int readfile(std::string path, std::string& body)
{
    std::ifstream stream;
    stream.open(path);
    if (!stream)
    {
        //TODO::Error
        return 404;
    }
    while (std::getline(stream, body))
    {}
    return 200;
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
        return "content-type: " + it->second;
    }

    return "Content-type: text/html";  // デフォルトのMIMEタイプ
}

std::string Response::getContentLength()
{
    std::ostringstream oss;
    oss << "Content-length: " << this->body.length() << "\r\n";
    return oss.str();
}

std::string Response::getDate()
{
    char buffer[100];
    time_t now = time(0);
    struct tm* gmt = gmtime(&now);
    
    strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", gmt);
    return "Date: " + std::string(buffer) + "\r\n";
}

std::string Response::getServer()
{
    return "Server: Apache/2.4.41\r\n";
}

std::string Response::getConnection()
{
    return "Connection: keep-Alive\r\n";
}

void Response::getStatusCode()
{
    if (this->statCode == 200)
        this->situation = "OK";
    else if (this->statCode == 301)
        this->situation = "Moved Permanetly";
    else if (this->statCode == 403)
        this->situation = "Forbidden";
    else if (this->statCode == 409)
        this->situation = "Conflict";
    else if (this->statCode == 204)
        this->situation = "No Content";
    else if (this->statCode == 500)
        this->situation = "Internal Server Error";
}       

void Response::wirteMessage(int socket)
{
    int reqline = this->request_line.length();
    int headerLen = this->header.length();
    int bodyLen = this->body.length();
    int totalLine = reqline + headerLen + bodyLen + 2;
    std::string total = this->request_line + this->header + "\r\n" +this->body;
    //TODO: Error handling
    write(socket, total.c_str(), totalLine);
}

std::vector<std::string> getContents(const std::string& path)
{
    std::vector<std::string> files;
    DIR* dir = opendir(path.c_str());

    if (dir != NULL)
    {
        struct dirent* entry;
        while ((entry = readdir(dir)) != NULL)
            files.push_back(entry->d_name);
        closedir(dir);
    }
    return files;
}

std::string generateDirectoryListing(const std::string& path, const std::vector<std::string>& files) {
    std::stringstream html;
    
    html << "<html>\n<head>\n"
         << "<title>Index of " << path << "</title>\n"
         << "</head>\n<body>\n"
         << "<h1>Index of " << path << "</h1>\n"
         << "<table>\n";
    
    for (const auto& file : files) {
        html << "<tr>\n"
             << "<td><a href=\"" << file << "\">" << file << "</a></td>\n"
             << "</tr>\n";
    }
    
    html << "</table>\n</body>\n</html>";
    return html.str();
}

std::string Response::getRequestLine()
{
    return "HTTP/1.1 " + std::to_string(this->statCode) + " " + this->situation + "\r\n";
}