#include "CGIHandler.hpp"

CGIHandler::CGIHandler() : envp(NULL), newBody("")
{
}

CGIHandler::CGIHandler(Request req, int epfd) : envp(NULL), newBody("")
{
    this->filePath = "/cgi/bin";
    this->req = req;
    this->InitCGIPath();
    this->epfd_ = epfd;
    this->child_pid = 0;
}

CGIHandler::CGIHandler(const CGIHandler& other) : envp(NULL), newBody("")
{
    this->env = other.env;
    this->filePath = other.filePath;
    this->req = other.req;
    this->epfd_ = other.epfd_;
    this->child_pid = other.child_pid;
}

CGIHandler& CGIHandler::operator=(const CGIHandler& other)
{
    if (this != &other) {
        this->env = other.env;
        this->filePath = other.filePath;
        this->req = other.req;
        this->epfd_ = other.epfd_;
        this->child_pid = other.child_pid;
    }
    return *this;
}

CGIHandler::~CGIHandler()
{
	if (this->envp) {
		for (size_t i = 0; this->envp[i] != NULL; ++i) {
			free(this->envp[i]);
		}
		delete[] this->envp;
		this->envp = NULL;
	}
}

void CGIHandler::InitCGIPath()
{
    char cwd[100];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("getcwd");
    }
    this->env["REDIRECT_STATUS"] = "200";
    this->env["REQUEST_METHOD"] = req.getMethod();
    this->env["CONTENT_LENGTH"] = req.getHeader()["Content-length"];
    this->env["CONTENT_TYPE"] = req.getHeader()["Content-Type"];
    this->env["QUERY_STRING"] = req.getQuery();
    this->env["REQUEST_URI"] = req.getUri() + req.getQuery();
    this->env["SERVER_PROTOCOL"] = "HTTP/1.1";
    this->env["SERVER_SOFTWARE"] = "Weebserv/1.0";
    this->getPathInfo();
    this->getEnvAsChar();
}

void CGIHandler::getPathInfo()
{
    std::string uri = this->req.getUri();
    int pos = uri.find(".py/");
    if (pos == -1)
    {
        pos = uri.find(".py");
		this->env["SCRIPT_NAME"] = uri.substr(1, pos + 3);
		this->env["PATH_INFO"] = "";
		return;
    }

    this->env["SCRIPT_NAME"] = uri.substr(1, pos + 3);
    this->env["PATH_INFO"] = uri.substr(pos + 3, uri.size());
}

void CGIHandler::getEnvAsChar()
{ 
    std::vector<char*> temp;

    for (std::map<std::string, std::string>::const_iterator it = this->env.begin(); it != this->env.end(); ++it) 
    {
        const std::pair<std::string, std::string>& pair = *it;
        std::string keyValue = pair.first + "=" + pair.second;
        temp.push_back(strdup(keyValue.c_str()));
    }
    temp.push_back(NULL);

	if (this->envp) {
		for (size_t i = 0; this->envp[i] != NULL; ++i) {
			free(this->envp[i]);
		}
		delete[] this->envp;
		this->envp = NULL;
	}

    this->envp = new char*[temp.size()];
    std::copy(temp.begin(), temp.end(), this->envp);
}

bool    CGIHandler::addContentLength(const std::string& httpResponse)
{
	size_t headerEndPos = httpResponse.find("\r\n\r\n");
    if (headerEndPos == std::string::npos)
        return false;
    std::string headers = httpResponse.substr(0, headerEndPos);
    std::string body = httpResponse.substr(headerEndPos + 4);

    std::istringstream headersStream(headers);
    std::vector<std::string> headerLines;
    std::string line;
    while (std::getline(headersStream, line)) {
        if (!line.empty()) {
            headerLines.push_back(line);
        }
    }

    std::ostringstream oSS;
    oSS << "Content-Length: " << body.size();
    headerLines.push_back(oSS.str());

    std::ostringstream modifiedHeaders;
    for (size_t i = 0; i < headerLines.size(); ++i) {
        modifiedHeaders << headerLines[i];
        if (i < headerLines.size() - 1) {
            modifiedHeaders << "\r\n";
        }
    }
    this->res = modifiedHeaders.str() + "\r\n\r\n" + body;

    return true;
}

int CGIHandler::CGIExecute()
{
    int pid;
    int fds[2];
    const char *inteprinter = "/usr/bin/python3";
    std::string py = "python3";
    std::string path = this->env["SCRIPT_NAME"];
    char *argv[] = {
        const_cast<char*>(py.c_str()),
        const_cast<char*>(path.c_str()),
        NULL
    };
    if (pipe(fds) == -1) {
        throw std::runtime_error("Failed to create pipe");
    }
    pid = fork();
	if (pid == -1) {
		throw std::runtime_error("Failed to fork");
	}
    if (pid > 0)
    {
        this->child_pid = pid;
		close(fds[1]);
		int flags = fcntl(fds[0], F_GETFL, 0);
		if (flags == -1) {
			throw std::runtime_error("Failed to get file descriptor flags");
		}
		if (fcntl(fds[0], F_SETFL, flags | O_NONBLOCK) == -1) {
			throw std::runtime_error("Failed to set file descriptor flags");
		}
    }
    else if (pid == 0)
    {
        close(fds[0]);
    	dup2(fds[1], 1);
        execve(inteprinter, argv, this->envp);
	    std::exit(1);
    }
	return fds[0];
}

void CGIHandler::appendCGIBody(const std::string &buffer)
{
	this->newBody += buffer;
}

std::string CGIHandler::getCGIBody()
{
	return this->newBody;
}

pid_t CGIHandler::getChildPid()
{
    return (this->child_pid);
}

std::string CGIHandler::getRes()
{
    return this->res;
}