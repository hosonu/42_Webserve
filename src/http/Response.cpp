#include "Response.hpp"

Response::Response()
{
    this->statCode = 0;
    this->situation = "";
    this->header = "";
    this->body = "";
    this->request_line = "";
    this->cgiBody = "";
}

Response::~Response()
{}

void Response::createMessage(Request &req, ServerConfig& conf)
{
    RequestValidConf validConf(req, conf);
    validConf.validReqLine();
    this->setStatusCode(req.getPrse().getTotalStatus(), validConf.getStat());
    this->truePath = this->createTruePath(conf, req.getUri());
    if (req.getUri() == "/cgi/bin/test.py")
    {
        CGIHandler executor(req);
        this->cgiBody = executor.CGIExecute();
        return ;
    }
    if (req.getMethod() == "GET")
        this->getBodyGet(this->truePath, conf);
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
}

//void Response::serachCgiPath()
//{
//    this.getCgi
//}

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

bool Response::checkMatching(std::string locPath, std::string uri)
{
    size_t count = 0;
    for (size_t i = 0; i < locPath.size(); i++)
    {
        if (locPath[i] == uri[i])
            count++;
    }
    if (count == locPath.size())
    {
        if (locPath == this->cgiPath)
        {
            this->cgiFlag = true;
        }
        return true;
    }
    else
    {
        return false;
    }
}

std::string Response::createTruePath(ServerConfig& conf, std::string uri)
{
    std::string res = uri;
    std::string indexFile;
    for (std::vector<Location>::const_iterator it = conf.getLocations().begin(); it != conf.getLocations().end(); ++it)
    {
        if ((checkMatching(it->getPath(), uri) == true))
        {
            this->serverLocation = *it;
            res = it->getRoot() + uri;
        }
    }
    return res;
}

std::string Response::createErrorPath(ServerConfig& conf, std::string& path)
{
    for (std::map<int, std::string>::const_iterator it = conf.getErrorPages().begin(); it != conf.getErrorPages().end(); ++it) 
    {
        if (it->first == this->statCode)
        {
            //最初のルートが'/'の前提
            std::string tmp = conf.getLocations().begin()->getRoot();
            if (tmp == "/")
            {
                path = tmp;
                path += it->second;
            }
            else
            {
                path = "/usr/share/nginx/html/404.html";
            }
        }
        else if (it == conf.getErrorPages().end())
        {
            path = "";
        }
    }
    return path;
}

std::string Response::addIndexFile(std::string res)
{
    if (checkFileType(res) == DIRECT)
    {
        res += this->serverLocation.getIndexFile();
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
        this->statCode = 404;
        this->body = createErrorPage(this->statCode, "Not Found");
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

    if (stat(path.c_str(), &statBuf) != 0) {
        this->statCode = 400;
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


void Response::getBodyGet(std::string path, ServerConfig& conf)
{
    std::string line;
    std::ifstream error(createErrorPath(conf, path).c_str());
    std::string index = addIndexFile(path);
    std::ifstream file(index.c_str());
    std::cout << index << std::endl;
    int type = checkFileType(index);
    if (this->statCode != 200)
    {
        readErrorFile(error);
        if (checkErroPath(conf) == false)
        {
            this->statCode = 404;
            this->getStatusCode();
            this->body = createErrorPage(this->statCode, this->situation);
        }
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
            readErrorFile(error);
            if (checkErroPath(conf) == false)
            {
                this->statCode = 404;
                this->getStatusCode();
                this->body = createErrorPage(this->statCode, this->situation);
            }
        }
    }
    else if (type == DIRECT)
    {
        //autoindexの項目がそもそもない場合の対応
        if (this->serverLocation.isAutoindex() == true && this->serverLocation.getIndexFile() == "")
        {
            this->body = generateDirectoryListing(path, getContents(path));
        }
        else
        {
            readErrorFile(error);
            if (checkErroPath(conf) == false)
            {
                this->statCode = 404;
                this->getStatusCode();
                this->body = createErrorPage(this->statCode, this->situation);
            }
        }

    }
}

std::string generateRandomFileName(std::string dir) 
{
    std::ostringstream oss;
    oss << dir << "/uploaded_file_" << time(0) << ".txt";
    std::cout << oss.str() << std::endl;
    return oss.str();
}

void Response::getBodyPost(Request& req, ServerConfig& conf)
{
    //TODO:configの設定を反映
    std::ifstream error(createErrorPath(conf, this->truePath).c_str());
    std::string fileName = generateRandomFileName(this->truePath);
    std::ofstream file(fileName.c_str());
    if (!file.is_open()) 
    {
        readErrorFile(error);
        if (checkErroPath(conf) == false)
        {
            this->statCode = 404;
            this->body = createErrorPage(this->statCode, "Not Found");
        }
        return ;
    }
    this->body = req.getBody();
    file << req.getBody();
    this->statCode = 200;
    file.close();
}

void Response::getBodyDel( ServerConfig& conf)
{
    std::ifstream error(createErrorPath(conf, this->truePath).c_str());
    int fType = checkFileType(this->truePath);
    if (fType == NORMAL_FILE)
    {
        if (std::remove(this->truePath.c_str()) == 0)
        {
            this->statCode = 201;
            return ;
        }
        else
        {
            //TODO:Errorページ設定
            readErrorFile(error);
            if (checkErroPath(conf) == false)
            {
                this->statCode = 404;
                this->body = createErrorPage(this->statCode, "Not Found");
            }
        }
    }
    else
    {
        this->statCode = 404;
        readErrorFile(error);
        if (checkErroPath(conf) == false)
        {
            this->statCode = 404;
            this->body = createErrorPage(this->statCode, "Not Found");
        }

    }
}

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
    else if (this->statCode == 409)
        this->situation = "Conflict";
    else if (this->statCode == 500)
        this->situation = "Internal Server Error";
}       

void Response::wirteMessage(int socket)
{
    if (cgiBody.empty())
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
    else
    {
        int length = this->cgiBody.length();
        write(socket, this->cgiBody.c_str(), length);
    }
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