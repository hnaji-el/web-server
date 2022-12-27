
#include "Request.hpp"

Request::Request(void)
	: state(STARTED), fd(-1), flag(HEADERS)
{
}

Request::Request(const int fd)
	: state(STARTED), fd(fd), flag(HEADERS)
{
}

Request::~Request(void)
{
}

/*
 * ENTRY POINT member function
 */

void	Request::operator()(const char* buff)
{
	if (!this->collectRequestHeaders(buff))
		return ;

	this->parseRequestHeaders();

	this->state = FINISHED;
}

bool	Request::collectRequestHeaders(const char* buff)
{
	this->buffer += buff;
	this->pos = this->buffer.find("\r\n\r\n");

	if (pos != std::string::npos)
		return (true);
	return (false);
}

void	Request::parseRequestHeaders(void)
{
	size_t	fPos = 0;
	size_t	lPos = 0;
	bool	reqLine = true;

	for (; fPos < this->pos; fPos = lPos + 2)
	{
		lPos = this->buffer.find("\r\n", fPos);
		if (reqLine == true)
		{
			this->parseRequestLine(fPos, lPos);
			reqLine = false;
		}
		else
			this->parseRequestHeader(fPos, lPos);
	}
}

void	Request::parseRequestLine(const size_t fPos, const size_t lPos)
{
	const size_t	fSP = this->buffer.find(" ", fPos);
	const size_t	sSP = this->buffer.find(" ", fSP + 1);

	this->headers["method"] = this->buffer.substr(fPos, fSP - fPos);
	this->headers["uri"] = this->buffer.substr(fSP + 1, sSP - fSP - 1);
	this->headers["http-version"] = this->buffer.substr(sSP + 1, lPos - sSP - 1);
}

void	Request::parseRequestHeader(const size_t fPos, const size_t lPos)
{
	const size_t		cPos = this->buffer.find(":", fPos);
	const std::string	fieldName = this->buffer.substr(fPos, cPos - fPos);
	const std::string	fieldValue = this->buffer.substr(cPos + 2, lPos - cPos - 2);

	this->headers[fieldName] = fieldValue;
}

void	Request::parseRequestBody(void)
{
}
















