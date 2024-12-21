#include "Client.hpp"

Client::Client(int fd, int epoll_fd)
: client_fd(fd), epfd(epoll_fd), cgi_fd(-1), mode(HEADER_READING) {
}

Client::~Client() {
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

void	Client::parseRequestHeader(std::vector<ServerConfig> &configData) {
	char	buffer[MAX_BUFEER];
	ssize_t count = read(this->client_fd, buffer, sizeof(buffer));
	// if (count == -1) {
	// 	std::cerr << "Failed to read : not enough memory" << std::endl;
	// 	this->mode = CLOSING;
	// 	return ;
	// }
	if (count > 0) {
		req.setRawHeader(std::string(buffer, count));
		std::string& current_header = req.getRawHeader();
		size_t header_end = current_header.find("\r\n\r\n");

		if (header_end != std::string::npos) {
			std::string body_part = current_header.substr(header_end + 4);
			current_header = current_header.substr(0, header_end + 4);

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
		// if (count == -1) {
		// 	std::cerr << "Failed to read : not enough memory" << std::endl;
		// 	this->mode = CLOSING;
		// 	return ;
		// }
		if (count > 0) {
			this->req.appendBody(std::string(buffer, count));
		}
		if (req.isBodyComplete()) {
			this->mode = WAITING;
		}
	}
}

void	Client::bindToConfig(std::vector<ServerConfig> &configData) {
	std::map<std::string, std::string> header = this->req.getHeader();
	std::string hostValue;
	std::map<std::string, std::string>::iterator it = header.find("Host");
	if (it != header.end()) {
		hostValue = it->second;
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
		port = 8080;
	}
	if (host == "localhost") {
		host = getLocalhostIpv4();
	}
	for(std::vector<ServerConfig>::iterator iter = configData.begin(); iter != configData.end(); ++iter)
	{
		if (iter->getServerName() == host) {
			this->configDatum = *iter;
		} else if (iter->getListenHost() == host && iter->getListenPort() == port) {
			this->configDatum = *iter;
		}
	}
}

void    Client::makeResponse()
{
	if (this->mode == WAITING) {
		if (this->msg.createMessage(this->req, this->configDatum) == "CGI_READING") {
			this->cgi = CGIHandler(this->req,epfd);
			this->cgi_fd = this->cgi.CGIExecute();
			this->child_pid = this->cgi.getChildPid();
			struct epoll_event ev;
			ev.events = EPOLLIN | EPOLLOUT;
			ev.data.ptr = this;
			if (epoll_ctl(this->epfd, EPOLL_CTL_ADD, this->cgi_fd, &ev) == -1) {
				throw std::runtime_error("Failed to add epoll");
			}
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

bool set_cgi_response(std::string cgibody, bool checkAddContent)
{
	if (cgibody.empty() || checkAddContent == false)
		return (false);
	else
		return (true);
}

void	Client::readCGI() {
	char	buffer[MAX_BUFEER];
	ssize_t count = read(this->cgi_fd, buffer, sizeof(buffer));
	if (count == -1) {
		std::cerr << "Failed to read : not enough memory" << std::endl;
		this->msg.setStatusCode(500, 500);
		std::ifstream error(this->msg.createErrorPath(this->configDatum).c_str());
		this->msg.readErrorFile(error);
		this->msg.set_headers(this->req);
		this->mode = WRITING;
	}
	if (count > 0) {
		this->cgi.appendCGIBody(std::string(buffer, count));
	}
	if (count == 0) {
		epoll_ctl(this->epfd, EPOLL_CTL_DEL, this->cgi_fd, NULL);
		close(this->cgi_fd);
		if (set_cgi_response(this->cgi.getCGIBody(), this->cgi.addContentLength(this->cgi.getCGIBody())))
			this->msg.setCGIBody("HTTP/1.1 200 OK\r\n" + this->cgi.getRes());
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

void Client::updateActivity() {
    this->last_activity = time(NULL);
}

bool Client::isTimedOut(time_t current_time, time_t timeout_seconds) const {
	return (current_time - last_activity) > timeout_seconds;
}

void Client::end_timeoutCGI()
{
	if (epoll_ctl(this->epfd, EPOLL_CTL_DEL, this->cgi_fd, NULL) == -1) {
		throw std::runtime_error("Failed to remove epoll");
	}
	close(this->cgi_fd);
	int status = kill(this->child_pid, SIGKILL);
	if (status == -1)
		perror("kill");
	this->msg.setStatusCode(500, 500);
    std::ifstream error(this->msg.createErrorPath(this->configDatum).c_str());
	this->msg.readErrorFile(error);
	this->msg.set_headers(this->req);
	this->mode = WRITING;
	this->updateActivity();
}

void Client::mode_timetowrite()
{
	this->msg.setStatusCode(408, 408);
    std::ifstream error(this->msg.createErrorPath(this->configDatum).c_str());
	this->msg.readErrorFile(error);
	this->msg.set_headers(this->req);
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
      req(other.req),
      msg(other.msg),
      cgi(other.cgi),
      configDatum(other.configDatum)
{
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
        req = other.req;
        msg = other.msg;
        cgi = other.cgi;
        configDatum = other.configDatum;
    }
    return *this;
}
