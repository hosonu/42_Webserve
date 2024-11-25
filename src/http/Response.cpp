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

void Response::createMessage(Request &req, ServerConfig& conf)
{
    RequestValidConf validConf(req, conf);
    validConf.validReqLine();
    this->setStatusCode(req.getPrse().getTotalStatus(), validConf.getStat());
    if (req.getMethod() == "GET")
        this->getBody(this->createTruePath(conf), conf);
    //else if (req.getMethod() == "POST")
    this->getStatusCode();
    this->request_line = this->getRequestLine();
    this->header += this->getContentType(req.getUri()) + "\r\n";
    this->header += this->getContentLength();
    this->header += this->getDate();
    this->header += this->getServer();
    this->header += this->getConnection();
}

void Response::setStatusCode(int parseNum, int confNum)
{
    if (parseNum != 200 && confNum != 200)
        this->statCode = parseNum;
    else if (parseNum == 200 && confNum != 200)
        this->statCode = confNum;
    else if (parseNum != 200 && confNum == 200)
        this->statCode = parseNum;
    else
        this->statCode = confNum;
}


std::string Response::createTruePath(ServerConfig& conf)
{
    std::string res = conf.getLocations().begin()->getRoot() + "/" +conf.getLocations().begin()->getIndexFile();
    std::cout << res << std::endl;
    return res;
}

std::string Response::createErrorPath(ServerConfig& conf)
{
    std::string res;
    for (std::map<int, std::string>::const_iterator it = conf.getErrorPages().begin(); it != conf.getErrorPages().end(); ++it) 
    {
        if (it->first == this->statCode)
            res = conf.getLocations().begin()->getRoot() + it->second;
    }
    return res;
}

std::string Response::createErrorPage(int statusCode, const std::string& statusMessage)
{
    std::ostringstream page;

    page << "<!DOCTYPE html>\n";
    page << "<html>\n";
    page << "<head><title>" << statusCode << " " << statusMessage << "</title></head>\n";
    page << "<body>\n";
    page << "<center><h1>" << statusCode << " " << statusMessage << "</h1></center>\n";
    page << "<hr>\n";
    page << "<center>My Simple Server</center>\n";
    page << "</body>\n";
    page << "</html>\n";

    return page.str();
}

int Response::checkFileType(const std::string& path) {
    struct stat statBuf;

    if (stat(path.c_str(), &statBuf) != 0) {
        std::cerr << "Error: " << strerror(errno) << std::endl;
        return -1;
    }

    if (S_ISREG(statBuf.st_mode)) {
        return 1;
    } else if (S_ISDIR(statBuf.st_mode)) {
        return 2;
    } else {
        return 0;
    }
}

void Response::getBody(const std::string& path, ServerConfig& conf)
{
    std::string line;
    std::ifstream error(createErrorPath(conf).c_str());
    std::ifstream file(path.c_str());
    int type = checkFileType(path);
    if (this->statCode != 200)
    {
        if (conf.getErrorPages().begin()->second != "")
        {
            if (error.is_open())
            {
                while (getline(error, line))
                {
                    this->body += line;
                }
            }
            else
            {
                this->body = createErrorPage(this->statCode, "Bad Reqeust");
            }
            error.close();
        }
        else
        {
            this->statCode = 404;
            this->body = createErrorPage(this->statCode, "Not Found");
        }
    }
    else if (type == NOMAL_FILE)
    {
        if (file.is_open())
        {
            while (getline(file, line))
            {
                this->body += line;
            }
            file.close();
        }
        else
        {
            if (conf.getErrorPages().begin()->second != "")
            {
                if (error.is_open())
                {
                    while (getline(error, line))
                    {
                        this->body += line;
                    }
                }
                else
                {
                    this->statCode = 404;
                    this->body = createErrorPage(this->statCode, "Not Found");
                }
                error.close();
            }
            else
            {
                this->statCode = 404;
                this->body = createErrorPage(this->statCode, "Not Found");
            }
        }
    }
    else if (type == DIRECT)
    {
        if (conf.getLocations().begin()->isAutoindex() == true && conf.getErrorPages().begin()->first == 0)
        {
            this->body = generateDirectoryListing(path, getContents(path));
        }
        else if (conf.getErrorPages().begin()->second != "")
        {
            this->statCode = 404;
            if (error.is_open())
            {
                while (getline(error, line))
                    this->body += line;
            }
            else
            {
                this->statCode = 404;
                this->body = createErrorPage(this->statCode, "Not Found");
            }
            error.close();
        }

    }
}

//int readfile(std::string path, std::string& body)
//{
//    std::ifstream stream;
//    stream.open(path.c_str());
//    if (!stream)
//    {
//        //TODO::Error
//        return 404;
//    }
//    while (std::getline(stream, body))
//    {}
//    return 200;
//}

std::string Response::getContentType(const std::string& filePath)
{
    std::string extension = filePath.substr(filePath.find_last_of(".") + 1);

    const char* extensions[] = {
        "html", "text/html",
        "htm", "text/html",
        "css", "text/css",
        "js", "application/javascript",
        "json", "application/json",
        "png", "image/png",
        "jpg", "image/jpeg",
        "jpeg", "image/jpeg",
        "gif", "image/gif",
        "svg", "image/svg+xml",
        "xml", "application/xml",
        "pdf", "application/pdf",
        "txt", "text/plain"
    };

	for (size_t i = 0; i < sizeof(extensions) / sizeof(extensions[0]); i += 2) {
		if (extension == extensions[i]) {
			return "Content-type: " + std::string(extensions[i + 1]);
		}
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
    else if (this->statCode == 400)
        this->situation = "Bad Reqeust";
    else if (this->statCode == 404)
        this->situation = "Not Found";
    else if (this->statCode == 403)
        this->situation = "Forbidden";
    else if (this->statCode == 405)
        this->situation = "Method Not Allowed";
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
	#ifdef DEBUG
	//std::cout << "total " << total << std::endl;
	#endif
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
	for (std::vector<std::string>::const_iterator it = files.begin(); it != files.end(); ++it) {
		html << "<tr>\n"
			<< "<td><a href=\"" << *it << "\">" << *it << "</a></td>\n"
			<< "</tr>\n";
	}
    //for (const auto& file : files) {
    //    html << "<tr>\n"
    //         << "<td><a href=\"" << file << "\">" << file << "</a></td>\n"
    //         << "</tr>\n";
    //}
    
    html << "</table>\n</body>\n</html>";
    return html.str();
}

std::string Response::getRequestLine()
{
    return "HTTP/1.1 " + customToString(this->statCode) + " " + this->situation + "\r\n";
}