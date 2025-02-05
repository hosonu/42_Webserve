#include "HttpParse.hpp"

HttpParse::HttpParse()
{

}

HttpParse::~HttpParse()
{

}

void HttpParse::checkReqLine(const std::string reqLine, std::string method, std::string uri, std::string ver)
{
	if (!this->checkSp(reqLine) || !this->checkMethod(method) || !this->checkUri(uri) || !this->checkVersion(ver))
		this->startStatus = 400;
	else
		this->startStatus = 200;
}

bool HttpParse::checkSp(const std::string reqLine)
{
	int spCount = 0;
	for(size_t i = 0; i < reqLine.size(); i++)
	{
		if (isspace(reqLine[i]))
			spCount++;
	}
	if (reqLine[reqLine.size() - 1] != '\r')
		return false;
	if (spCount == 3)
		return true;
	else
		return false;
}

bool HttpParse::checkMethod(std::string method)
{
	if (method == "GET" || method == "POST" || method == "DELETE")
		return true;
	else
		return false;
}

bool HttpParse::checkUri(std::string &uri)
{
	if (uri.size() == 1 && uri[0] == ' ')
		return false;
	if (uri.find("%") != std::string::npos)
		uri = decodeURI(uri);
	return true;
}

std::string decodeURI(const std::string& encodedURI) {
    std::string result;
    std::string hexStr;

    for (size_t i = 0; i < encodedURI.length(); ++i) {
        if (encodedURI[i] == '%' && i + 2 < encodedURI.length() &&
            std::isxdigit(encodedURI[i + 1]) && std::isxdigit(encodedURI[i + 2])) {
            hexStr = encodedURI.substr(i + 1, 2);
            std::istringstream iss(hexStr);
            int hexValue;
            iss >> std::hex >> hexValue;
            result += static_cast<char>(hexValue);
            i += 2;
        } else if (encodedURI[i] == '+') {
            result += ' ';
        } else {
            result += encodedURI[i];
        }
    }
    return result;
}

std::string replaceSpecialChars(const std::string& decodedURI) {
    std::map<std::string, std::string> replacements;
    replacements[":"] = "スキームのセパレータ";
    replacements["/"] = "パスセパレータ";
    replacements["?"] = "クエリの開始";
    replacements["#"] = "フラグメント識別子";
    replacements["&"] = "パラメータの区切り";
    replacements["="] = "キーと値の区切り";

    std::string result = decodedURI;

    for (std::map<std::string, std::string>::const_iterator it = replacements.begin(); it != replacements.end(); ++it) {
        size_t pos = 0;
        while ((pos = result.find(it->first, pos)) != std::string::npos) {
            result.replace(pos, it->first.length(), it->second);
            pos += it->second.length();
        }
    }
    return result;
}

bool HttpParse::checkVersion(std::string ver)
{
	std::string tmp = ver;
	std::string name = tmp.erase(4);
	size_t slash = ver.find("/");

	if (name != "HTTP" || slash == std::string::npos)
	{
		return false;
	}
	std::string digit = ver.substr(ver.find("/") + 1);
	if (digit.size() != 4)
	{
		return false;
	}
	if (!std::isdigit(digit[0]))
		return false;
	if (!std::isdigit(digit[digit.size() - 2]))
		return false;
	if (digit[1] != '.')
		return false;
	return true;
}

bool HttpParse::checkStructure(std::string headLine, std::map<std::string, std::string>& headers)
{
	size_t colon = headLine.find(":");
	std::string key;
	std::string val;
	
	if (colon == std::string::npos)
	{
		if (headLine[0] == '\r')
			return true;
		else
			return false;
	}
	if (headLine[headLine.size() - 1] != '\r')
	{
		this->headerStatus = 400;
		return false;
	}
	if (!isspace(headLine[colon + 1]))
	{
		this->headerStatus = 400;
		return false;
	}
	key = headLine.substr(0, colon);
    val = headLine.substr(colon + 1);
    val.erase(0, val.find_first_not_of(" \t"));
    val.erase(val.find_last_not_of(" \t") + 1);	
	if (!isValidHeaderFieldName(key) || !isValidHeaderFieldValue(val))
	{
		this->headerStatus = 400;
		return false;
	}
	headers[key] = val;
	this->headerStatus = 200;
	return true;	
}

void HttpParse::unChunckedBody(std::string body, std::string& unChunk)
{
	std::istringstream stream(body);
    std::string decodedBody;
    std::string line;

    while (std::getline(stream, line)) {
        std::istringstream sizeStream(line);
        size_t chunkSize = 0;
        sizeStream >> std::hex >> chunkSize;

        if (chunkSize == 0) {
            break;
        }

        char *buffer = new char[chunkSize];
        stream.read(buffer, chunkSize);
        decodedBody.append(buffer, chunkSize);
        delete[] buffer;
        stream.ignore(1);
    }
	unChunk = decodedBody;
}

void HttpParse::setTotalStatus()
{
	if (this->startStatus != 200 && this->headerStatus != 200) {
        this->totalStatus = this->startStatus;
	} else if (startStatus == 200 && headerStatus != 200) {
        this->totalStatus = this->headerStatus;
	} else if (startStatus != 200 && headerStatus == 200) {
        this->totalStatus = this->startStatus;
	} else {
        this->totalStatus = this->startStatus;
	}
}

void HttpParse::setHeaderStatus(int set)
{
	this->headerStatus = set;
}

int HttpParse::getHeaderStatus()
{
	return this->headerStatus;
}

int HttpParse::getStartStatus()
{
	return this->startStatus; 
}

int HttpParse::getTotalStatus()
{
	return this->totalStatus;
}

bool isValidHeaderFieldName(const std::string& fieldName) 
{
    std::string::const_iterator it;
    for (it = fieldName.begin(); it != fieldName.end(); ++it) {
        if (!(std::isalnum(*it) || *it == '-' || *it == '_')) {
            return false;
        }
    }
    return true;
}

bool isValidHeaderFieldValue(const std::string& fieldValue)
{
    std::string::const_iterator it;
    for (it = fieldValue.begin(); it != fieldValue.end(); ++it) {
        if (std::iscntrl(*it) == true) {
            return false;
        }
    }
    return true;
}