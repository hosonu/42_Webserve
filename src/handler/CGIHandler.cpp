#include "CGIHandler.hpp"

CGIHandler::CGIHandler()
{
    //config依存　ファイルパス socket rootのパス
    //test 
    //this->filePath = "/cgi/bin";
    //this->InitCGIPath();
}

CGIHandler::CGIHandler(Request req, int epfd) : newBody("")
{
    this->filePath = "/cgi/bin";
    this->req = req;
    this->InitCGIPath();
    this->epfd_ = epfd;
}


CGIHandler::~CGIHandler()
{}

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
    this->envp = new char*[temp.size()];
    std::copy(temp.begin(), temp.end(), this->envp);
}


void freeCharArray(char** array) {
    if (!array) return;
    for (size_t i = 0; array[i] != NULL; ++i) {
        // free(array[i]);
    }
    delete[] array;
}

std::string	CGIHandler::addContentLength(const std::string& httpResponse)
{
    //if (_filePath.substr(_filePath.find_last_of('.') + 1) != "py") return (httpResponse);
    
	size_t headerEndPos = httpResponse.find("\r\n\r\n");
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

    return modifiedHeaders.str() + "\r\n\r\n" + body;
}

int CGIHandler::CGIExecute(Client *client)
{
    //TODO:cgiが失敗したばあい(解決予定)
    //cgiが失敗しても特になにもしないで正解っぽい(以下ryanagit)
    //execveの成功でメッセージが変わることはないっぽい
    //timeoutは本体で回収するのみ
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
    pipe(fds);
    pid = fork();
    if (pid > 0)
    {
		close(fds[1]);

		int flags = fcntl(fds[0], F_GETFL, 0);
		fcntl(fds[0], F_SETFL, flags | O_NONBLOCK);
        //これ必要じゃね(ryanagit)
		wait(NULL);
		//(void)epoll_fd;
		//struct epoll_event	ev;
		//ev.events = EPOLLIN | EPOLLOUT;
		//ev.data.fd = fds[0];
	    //ev.data.ptr = client;
		//epoll_ctl(epfd_, EPOLL_CTL_ADD, fds[0], &ev);
        (void) client;
    }
    else if (pid == 0)
    {
        close(fds[0]);
        dup2(fds[1], 1);
        execve(inteprinter, argv, this->envp);
        //read/writeしたわけではないのでのerronoは見るが回収しない(ryanagit)
	    std::exit(errno);
    }
	std::cout << "CGI_EXECUTE: " << fds[0] << std::endl;
	return fds[0];
}

void CGIHandler::appendCGIBody(char *buffer)
{
	this->newBody += buffer;
}

std::string CGIHandler::getCGIBody()
{
	return this->newBody;
}