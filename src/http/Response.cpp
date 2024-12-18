#include "Response.hpp"

Response::Response()
: statCode(0), cgiFlag(false), request_line(""), header(""), body(""), cgiBody(""), truePath("a")
{
}

Response::~Response()
{}

std::string Response::createMessage(Request &req, ServerConfig& conf)
{
    this->truePath = this->createTruePath(conf, req.getUri());
    RequestValidConf validConf(req, this->serverLocation);
    validConf.validReqLine();
    this->setStatusCode(req.getPrse().getTotalStatus(), validConf.getStat());
    this->truePath = this->createTruePath(conf, req.getUri());
    if (this->cgiFlag && this->truePath.find(".py") !=std::string::npos) {
        return "CGI_READING";
    }
    if (req.getMethod() == "GET")
        this->getBodyGet(this->truePath, conf, req.getUri());
    else if (req.getMethod() == "POST") 
        this->getBodyPost(req, conf);
    else if (req.getMethod() == "DELETE")
        this->getBodyDel(conf);
    this->getStatusCode();
    this->request_line = this->getRequestLine();
    this->header += this->getContentType(req.getUri()) + "\r\n";
    this->header += this->getContentLength();
    this->header += this->getDate();
    this->header += this->getServer();
    this->header += this->getConnection();
	return "WRITING";
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

bool Response::checkMatching(const std::string& locPath, const std::string& uri)
{
    if (locPath.size() > uri.size())
        return false;
    return uri.compare(0, locPath.size(), locPath) == 0;
}
std::string Response::createTruePath(ServerConfig& conf, const std::string& uri)
{
    std::string bestMatch = uri;
    size_t longestMatch = 0;

    for (std::vector<Location>::const_iterator it = conf.getLocations().begin(); it != conf.getLocations().end(); ++it)
    {
        if (checkMatching(it->getPath(), uri))
        {
            if (it->getPath().size() > longestMatch)
            {
                longestMatch = it->getPath().size();
                bestMatch = it->getRoot() + uri;
                this->serverLocation = *it;
                this->cgiFlag = it->flagCGI();
            }
        }
    }
    return bestMatch;
}

std::string Response::createErrorPath(ServerConfig& conf)
{
    std::string res;
    for (std::map<int, std::string>::const_iterator it = conf.getErrorPages().begin(); it != conf.getErrorPages().end(); ++it) 
    {
        if (it->first == this->statCode)
        {
            res = this->serverLocation.getRoot() + it->second;
        }
        else if (it == conf.getErrorPages().end())
        {
            res = "";
        }
    }
    return res;
}

std::string Response::addIndexFile(std::string res)
{
    std::string tmp = res;
    if (checkFileType(res) == DIRECT)
    {
        tmp += this->serverLocation.getIndexFile();
        if (checkFileType(tmp) == NORMAL_FILE)
        {
            res += this->serverLocation.getIndexFile();
        }
    }
    return res;
}

void Response::readErrorFile(std::ifstream& error)
{
    std::string line;

    if (error.is_open())
    {
        while (getline(error, line))
        {
            this->body += line;
        }
    }
    else
    {
        this->body = createErrorPage(this->statCode, this->situation);
    }
    error.close();
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

int Response::checkFileType(std::string path) {
    struct stat statBuf;

    if (stat(path.c_str(), &statBuf) != 0) 
    {
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

bool Response::checkErroPath(ServerConfig& conf)
{
    for (std::map<int, std::string>::const_iterator it = conf.getErrorPages().begin(); it != conf.getErrorPages().end(); ++it) 
    {
        if (it->first == this->statCode)
            return true;
    }
    return false;
}


void Response::getBodyGet(std::string path, ServerConfig& conf, std::string uri)
{
    std::string line;
    std::string index = addIndexFile(path);
    std::ifstream file(index.c_str());
    int type = checkFileType(index);
    if (!this->serverLocation.getReturnPath().empty())
    {
        this->statCode = 301;
        this->header += "Location: " + this->serverLocation.getReturnPath() + + "\r\n";
        this->getStatusCode();
    }
    else if (this->statCode != 200)
    {
        std::ifstream error(createErrorPath(conf).c_str());
        readErrorFile(error);
    }
    else if (type == NORMAL_FILE)
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
            this->statCode = 403;
            std::ifstream error(createErrorPath(conf).c_str());
            readErrorFile(error);
        }
    }
    else if (type == DIRECT)
    {
        if (this->serverLocation.isAutoindex() == true)
        {
            this->body = generateDirectoryListing(path, getContents(path, uri));
        }
        else
        {
            this->statCode = 404;
            std::ifstream error(createErrorPath(conf).c_str());
            readErrorFile(error);
        }

    }
    else
    {
        this->statCode = 404;
        std::ifstream error(createErrorPath(conf).c_str());
        readErrorFile(error);
    }
}

std::string generateRandomFileName(std::string dir) 
{
    std::ostringstream oss;
    oss << dir << "/uploaded_file_" << time(0) << ".txt";
    std::cout << oss.str() << std::endl;
    return oss.str();
}

size_t convert_stos(const std::string& max_body_size) 
{
    if (max_body_size.empty())
        return (-1);
    std::string num = max_body_size.substr(0, max_body_size.length() - 1);
    char unit = max_body_size[max_body_size.length() - 1];

    char* endptr = NULL;
    size_t size = std::strtol(num.c_str(), &endptr, 10);

    switch (unit) {
        case 'k':
            return size * 1024;
        case 'm':
            return size * 1024 * 1024;
        case 'g':
            return size * 1024 * 1024 * 1024;
    }
    return (-1);
}

void Response::getBodyPost(Request& req, ServerConfig& conf)
{
    std::ifstream error(createErrorPath(conf).c_str());
    if (this->statCode != 200)
    {
        readErrorFile(error);
        return ;
    }
    if (!this->serverLocation.getReturnPath().empty())
    {
        this->statCode = 301;
        this->header += "Location: " + this->serverLocation.getReturnPath() + + "\r\n";
        this->getStatusCode();
        return ;
    }
    else if (req.getBody().empty())
    {
        this->statCode = 400;
        readErrorFile(error);
        return ;
    }
    else if (convert_stos(conf.getMaxBodySize()) > 0 && req.getBody().size() > convert_stos(conf.getMaxBodySize()))
    {
        this->statCode = 413;
        readErrorFile(error);
        return ;
    }
    std::string fileName = generateRandomFileName(this->truePath);
    std::ofstream file(fileName.c_str());
    if (!file.is_open()) 
    {
        this->statCode = 400;
        readErrorFile(error);
        return ;
    }
    this->body = req.getBody();
    file << req.getBody();
    this->statCode = 201;
    file.close();
}

void Response::getBodyDel( ServerConfig& conf)
{
    std::ifstream error(createErrorPath(conf).c_str());
    int fType = checkFileType(this->truePath);
    if (this->statCode != 200)
    {
        readErrorFile(error);
        return ;
    }
    if (fType == NORMAL_FILE)
    {
        if (std::remove(this->truePath.c_str()) == 0)
        {
            this->statCode = 204;
            return ;
        }
        else
        {
            this->statCode = 400;
            readErrorFile(error);
        }
    }
    else
    {
        this->statCode = 404;
        readErrorFile(error);
    }
}

std::string Response::getContentType(const std::string& filePath)
{
    size_t pos = filePath.find_last_of(".");
    if (pos == std::string::npos || pos == filePath.size() - 1) {
       return "Content-type: text/html";
    }
    std::string extension = filePath.substr(pos + 1);

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

    return "Content-type: text/html";
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
    return "Server: Webserve\r\n";
}

std::string Response::getConnection()
{
    return "Connection: keep-Alive\r\n";
}

void Response::getStatusCode()
{
    if (this->statCode == 200)
        this->situation = "OK";
    else if (this->statCode == 201)
        this->situation = "Created";
    else if (this->statCode == 204)
        this->situation = "No content";
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
    else if (this->statCode == 408)
        this->situation = "Request Timeout";
    else if (this->statCode == 409)
        this->situation = "Conflict";
    else if (this->statCode == 413)
        this->situation = "Payload Too Large";
    else if (this->statCode == 500)
        this->situation = "Internal Server Error";
}       

void Response::wirteMessage(int socket)
{
    if (cgiBody.empty() || this->statCode == 500)
    {
        int reqline = this->request_line.length();
        int headerLen = this->header.length();
        int bodyLen = this->body.length();
        int totalLine = reqline + headerLen + bodyLen + 2;
        std::string total = this->request_line + this->header + "\r\n" +this->body;

        if (write(socket, total.c_str(), totalLine) == -1) {
			std::cerr << "Failed to write" << std::endl;
        }
    }
    else
    {
        int length = this->cgiBody.length();
        if (write(socket, this->cgiBody.c_str(), length) == -1) {
			std::cerr << "Failed to write" << std::endl;
		}
    }
}

std::vector<std::string> getContents(const std::string& path, const std::string& uri) {
    std::vector<std::string> files;
    DIR* dir = opendir(path.c_str());
    if (dir != NULL) {
        struct dirent* entry;
        while ((entry = readdir(dir)) != NULL) {
            std::string name = entry->d_name;
            if (name != "." && name != "..") {
                std::string fullUri = uri;
                if (!fullUri.empty() && fullUri[fullUri.size() - 1] != '/') {
                    fullUri += "/";
                }
                fullUri += name;
                files.push_back(fullUri);
            }
        }
        closedir(dir);
    }
    return files;
}

std::string generateDirectoryListing(const std::string& uri, const std::vector<std::string>& files) {
    std::stringstream html;

    html << "<html>\n<head>\n"
         << "<title>Index of " << uri << "</title>\n"
         << "<style>\n"
         << "body { font-family: Arial, sans-serif; margin: 20px; }\n"
         << "h1 { color: #333; }\n"
         << "table { width: 100%; border-collapse: collapse; }\n"
         << "th, td { padding: 8px 12px; border: 1px solid #ddd; text-align: left; }\n"
         << "th { background-color: #f4f4f4; }\n"
         << "tr:hover { background-color: #f9f9f9; }\n"
         << "a { text-decoration: none; color: #007BFF; }\n"
         << "a:hover { text-decoration: underline; }\n"
         << "</style>\n"
         << "</head>\n<body>\n";

    html << "<h1>Index of " << uri << "</h1>\n";

    html << "<table>\n"
         << "<tr>\n"
         << "<th>Name</th>\n"
         << "</tr>\n";

    for (std::vector<std::string>::const_iterator it = files.begin(); it != files.end(); ++it) {
        std::string fileName = *it;
        std::size_t lastSlash = fileName.find_last_of('/');
        if (lastSlash != std::string::npos) {
            fileName = fileName.substr(lastSlash + 1);
        }

        html << "<tr>\n"
             << "<td><a href=\"" << *it << "\">" << fileName << "</a></td>\n"
             << "</tr>\n";
    }


    html << "</table>\n</body>\n</html>";

    return html.str();
}

std::string Response::getRequestLine()
{
    return "HTTP/1.1 " + customToString(this->statCode) + " " + this->situation + "\r\n";
}

void Response::setCGIBody(std::string body)
{
	this->cgiBody = body;
}

void Response::set_headers(Request &req)
{

    this->request_line = this->getRequestLine();
    this->header += this->getContentType(req.getUri()) + "\r\n";
    this->header += this->getContentLength();
    this->header += this->getDate();
    this->header += this->getServer();
    this->header += this->getConnection();
}
