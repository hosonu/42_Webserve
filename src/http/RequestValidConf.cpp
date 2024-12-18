#include "RequestValidConf.hpp"

RequestValidConf::RequestValidConf()
{}

RequestValidConf::RequestValidConf(Request& req, Location& loc)
{
    this->req = req;
    this->loc = loc;
}

RequestValidConf::~RequestValidConf()
{}

void RequestValidConf::validReqLine()
{
    std::string method = req.getMethod();
    std::vector<std::string> allow = this->loc.getAllowMethods();
    if (method == "GET" || method == "POST" || method == "DELETE")
    {
        if (checkAllow(method, allow))
            this->stat = 200;
        else
            this->stat = 405;
    }
    else
    {
        this->stat = 400;
    }
}

bool RequestValidConf::checkAllow(std::string method, std::vector<std::string>& allows)
{
    for (std::vector<std::string>::size_type i = 0; i < allows.size(); ++i) 
    {
        if (method == allows[i])
            return true;
    }
    return false;
}

int RequestValidConf::getStat()
{
    return this->stat;
}