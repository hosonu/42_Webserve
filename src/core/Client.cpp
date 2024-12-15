#include "Client.hpp"

Client::Client(int fd, int epoll_fd)
: client_fd(fd), epfd(epoll_fd), cgi_fd(-1), mode(HEADER_READING) {
}

void    Client::setMode(ClientMode mode) {
    this->mode = mode;
}

int Client::getClientFd() const{
    return this->client_fd;
}

const ClientMode&  Client::getClientMode() const {
    return  this->mode;
}

const ServerConfig	&Client::getConfigDatum() const {
	return	this->configDatum;
}

int Client::getCGIfd() const {
	return this->cgi_fd;
}

void Client::updateEpollEvent() {
	struct epoll_event ev;
	ev.events = EPOLLIN | EPOLLOUT | EPOLLET;
	ev.data.ptr = this;
	epoll_ctl(this->epfd, EPOLL_CTL_MOD, this->client_fd, &ev);
}

void	Client::parseRequestHeader(std::vector<ServerConfig> &configData) {
	char	buffer[MAX_BUFEER];
	ssize_t count = read(this->client_fd, buffer, sizeof(buffer));
	if (count > 0) {
		req.setRawHeader(std::string(buffer, count));
		std::string& current_header = req.getRawHeader();
		size_t header_end = current_header.find("\r\n\r\n");

		if (header_end != std::string::npos) {
			std::string body_part = current_header.substr(header_end + 4);
			current_header = current_header.substr(0, header_end + 4);
			
			#ifdef DEBUG
			//std::cout << "header: \n" << current_header << std::endl;
			#endif

			if (req.requestParse(req.getRawHeader()) == false) {
				std::cerr << "Bad Format: Header is not correct format" << std::endl;
			}

			if (!body_part.empty()) {	
				req.setBody(body_part);
			}
			this->bindToConfig(configData);
			this->mode = BODY_READING;
		}
	}
}

void	Client::parseRequestBody() {
	if (req.checkBodyExist() == false) {
		this->mode = WAITING;
	} else {
		char	buffer[MAX_BUFEER];
		ssize_t count = read(this->client_fd, buffer, sizeof(buffer));
		if (count > 0) {
			this->req.appendBody(buffer);
		}
		if (req.isBodyComplete()) {
			this->mode = WAITING;
		}
	}
}

void	Client::bindToConfig(std::vector<ServerConfig> &configData) {
	#ifdef DEBUG
 	//print_line(req);
	#endif
	std::map<std::string, std::string> header = this->req.getHeader();
	std::string hostValue;
	std::map<std::string, std::string>::iterator it = header.find("Host");
	#ifdef DEBUG
		//std::cout << "Request Header: " << req.getRawHeader() << std::endl;
		std::cout << "Parsed Host: " << it->second << std::endl;
	#endif
	if (it != header.end()) {
		hostValue = it->second;
		#ifdef DEBUG
		//std::cout << "Host: " << hostValue << std::endl;
		#endif
	} else {
		std::cerr << "None Header Host " << std::endl;
	}

	size_t colonPos = hostValue.find(':');
	std::string host;
	int port;
	if (colonPos != std::string::npos) {
		host = hostValue.substr(0, colonPos);
		port = std::strtol(hostValue.substr(colonPos + 1).c_str(), 0, 10);
	} else {
		host = hostValue;
		port = 8080;//should set default port number
	}

	for(std::vector<ServerConfig>::iterator iter = configData.begin(); iter != configData.end(); ++iter)
	{
		#ifdef DEBUG
		//std::cout << "ServerConfig: " << iter->serverName << ", " << iter->host << " : " << iter->listenPort << std::endl; 
		#endif
		if (iter->getServerName() == host) {
			this->configDatum = *iter;
		} else if (iter->getListenHost() == host && iter->getListenPort() == port) {
			this->configDatum = *iter;
		}
	}

	#ifdef DEBUG
	{
		//ServerConfig debug_data = this->getConfigDatum();
		//std::cout << "configDatum: " << "client fd: " << client_fd << ", " 
		//		<< "is_default: " << debug_data << ", "
		//		<< "listenPort: " << debug_data.listenPort << ", "
		//		<< "host: " << debug_data.host << ", "
		//		<< "serverName: " << debug_data.serverName << ", "
		//		<< "maxBodySize: " << debug_data.maxBodySize << std::endl;
	}
	#endif
}

void    Client::methodProc()
{
	if (this->mode == WAITING) {
		if (this->msg.createMessage(this->req, this->configDatum) == "CGI_READING") {
			this->cgi = CGIHandler(this->req,epfd);
			//dup2(this->cgi.CGIExecute(this->epfd), this->cgi_fd);
			this->cgi_fd = this->cgi.CGIExecute();
			this->child_pid = this->cgi.getChildPid();
			struct epoll_event ev;
			ev.events = EPOLLIN | EPOLLOUT;
			ev.data.ptr = this;
			epoll_ctl(this->epfd, EPOLL_CTL_ADD, this->cgi_fd, &ev);
			this->mode = CGI_READING;
		} else {
			this->mode = WRITING;
		}
	}
	if (this->mode == WRITING) {
		this->msg.wirteMessage(this->client_fd);
		this->mode = CLOSING;
	}
}

void    Client::makeResponse() {
	#ifdef DEBUG
 	//print_line(req);
	//print_conf(configDatum);
	//std::cout << req.getBody() << std::endl;
	#endif
    this->methodProc();
}


bool set_cgi_response(std::string cgibody)
{
	if (cgibody.empty())
		return (false);
	else
		return (true);
}

void	Client::readCGI() {
	char	buffer[MAX_BUFEER];
	ssize_t count = read(this->cgi_fd, buffer, sizeof(buffer));
	if (count > 0) {
		this->cgi.appendCGIBody(buffer);
	}
	if (count == 0) {
		epoll_ctl(this->epfd, EPOLL_CTL_DEL, this->cgi_fd, NULL);
		close(this->cgi_fd);
		if (set_cgi_response(this->cgi.getCGIBody()))
			this->msg.setCGIBody("HTTP/1.1 200 OK\r\n" + this->cgi.addContentLength(this->cgi.getCGIBody()));
		else
		{
			this->msg.setStatusCode(500, 500);
            std::ifstream error(this->msg.createErrorPath(this->configDatum).c_str());
            this->msg.readErrorFile(error);
			this->msg.set_headers(this->req);
		}
		this->mode = WRITING;
	}
}

void Client::end_timeoutCGI()
{
	int status = kill(this->child_pid,SIGKILL);
	if (status == -1)
		perror("kill");
	this->msg.setStatusCode(500, 500);
	this->msg.set_headers(this->req);
    std::ifstream error(this->msg.createErrorPath(this->configDatum).c_str());
	this->msg.readErrorFile(error);
	this->mode = WRITING;
	this->updateActivity();
}

Client::Client(const Client& other)
    : client_fd(other.client_fd),
      epfd(other.epfd),
      cgi_fd(other.cgi_fd),
      mode(other.mode),
      last_activity(other.last_activity),
      read_buffer(other.read_buffer),
      write_buffer(other.write_buffer),
      rawReq(other.rawReq),
      req(other.req),  // Request と Response がコピー可能であれば
      msg(other.msg),  // 同様に、コピーコンストラクターがあれば
      cgi(other.cgi),  // CGIHandler のコピーコンストラクターがあれば
      configDatum(other.configDatum)  // ServerConfig のコピーコンストラクターがあれば
{
    // 必要に応じて追加の処理
}

Client& Client::operator=(const Client& other)
{
    if (this != &other) {
        client_fd = other.client_fd;
        epfd = other.epfd;
        cgi_fd = other.cgi_fd;
        mode = other.mode;
        last_activity = other.last_activity;
        read_buffer = other.read_buffer;
        write_buffer = other.write_buffer;
        rawReq = other.rawReq;
        req = other.req;  // Request と Response がコピー可能であれば
        msg = other.msg;  // 同様に、コピーコンストラクターがあれば
        cgi = other.cgi;  // CGIHandler のコピーコンストラクターがあれば
        configDatum = other.configDatum;  // ServerConfig のコピーコンストラクターがあれば
    }
    return *this;
}