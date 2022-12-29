
#include "Request.hpp"
#include <ctime>

Request::Request(void)
	: headers(), state(STARTED), fileName("./configFileExamples/temp/file"),
	fd(-1), resFlag(HEADERNOTSENT), resState(BODYNOTSENT), fdBody(-1),
	contentLen(0), ActualContentLen(0),lastTimeUsed(time(NULL)), fileStream(), flag(HEADERS),
	chunkState(CHUNKSIZE), chunkSize(0), pos(0), buffer(), temp()
{
}

Request::Request(const int& fd)
	: headers(), state(STARTED), fileName("./configFileExamples/temp/file"),
	fd(fd), resFlag(HEADERNOTSENT), resState(BODYNOTSENT), fdBody(-1),
	contentLen(0), ActualContentLen(0),lastTimeUsed(time(NULL)), fileStream(), flag(HEADERS),
	chunkState(CHUNKSIZE), chunkSize(0), pos(0), buffer(), temp()
{
}

Request::Request(const Request& src)
{
	this->headers = src.headers;
	this->state = src.state;
	this->fileName = src.fileName;
	this->fd = src.fd;
	this->resFlag = src.resFlag;
	this->resState = src.resState;
	this->fdBody = src.fdBody;
	this->contentLen = src.contentLen;
	this->ActualContentLen = src.ActualContentLen;
	this->lastTimeUsed = src.lastTimeUsed;
	this->flag = src.flag;
	this->chunkState = src.chunkState;
	this->chunkSize = src.chunkSize;
	this->pos = src.pos;
	this->buffer = src.buffer;
	this->temp = src.temp;
}

Request&	Request::operator=(const Request& rhs)
{
	if (this == &rhs)
		return (*this);

	this->headers = rhs.headers;
	this->state = rhs.state;
	this->fileName = rhs.fileName;
	this->fd = rhs.fd;
	this->resFlag = rhs.resFlag;
	this->resState = rhs.resState;
	this->fdBody = rhs.fdBody;
	this->contentLen = rhs.contentLen;
	this->ActualContentLen = rhs.ActualContentLen;
	this->lastTimeUsed = rhs.lastTimeUsed;
	this->flag = rhs.flag;
	this->chunkState = rhs.chunkState;
	this->chunkSize = rhs.chunkSize;
	this->pos = rhs.pos;
	this->buffer = rhs.buffer;
	this->temp = rhs.temp;

	return (*this);
}

Request::~Request(void)
{
}

void	Request::clear(void)
{
	*this = Request(this->fd);
}

/*
 * ENTRY POINT member function
 */

void	Request::operator()(const char* chunk)
{
	if (flag == HEADERS)
	{
		if (!this->isRequestHeadersComplete(chunk))
			return ;
		this->parseRequestHeaders();
		chunk = this->checkAndSetFlags();
	}
	if (flag == BODY)
		this->parseRequestBodyWithoutEncoding(chunk);
	if (flag == BODYCHUNKED)
		this->parseRequestBodyWithEncoding(chunk);
}

bool	Request::isRequestHeadersComplete(const char* chunk)
{
	this->buffer += chunk;
	this->pos = this->buffer.find("\r\n\r\n");

	if (this->pos != std::string::npos)
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
			this->parseRequestLine(lPos);
			reqLine = false;
		}
		else
			this->parseRequestHeader(fPos, lPos);
	}
}

void	Request::parseRequestLine(const size_t lPos)
{
	const size_t	fSP = this->buffer.find(" ", 0);
	const size_t	sSP = this->buffer.find(" ", fSP + 1);

	this->headers["method"] = this->buffer.substr(0, fSP);
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

const char*	Request::checkAndSetFlags(void)
{
	if (this->headers["method"] != "POST" || !this->headers.count("Content-Length"))
		return (this->setState(FINISHED));
	if (this->headers.count("Transfer-Encoding") &&
		this->headers["Transfer-Encoding"] != "chunked")
		return (this->setState(FINISHED));

	this->flag = BODYCHUNKED;
	if (!this->headers.count("Transfer-Encoding"))
		this->flag = BODY;

	this->setFileName();
	this->fileStream.open(this->fileName);
	this->contentLen = this->stringToNumber(this->headers["Content-Length"]);
	this->temp = this->buffer.substr(this->pos + 4, std::string::npos);
	this->buffer.clear();

	return (this->temp.c_str());
}

void	Request::parseRequestBodyWithoutEncoding(const std::string& chunk)
{
	this->fileStream << chunk;
	this->ActualContentLen += chunk.size();
	if (this->ActualContentLen >= this->contentLen)
	{
		fileStream.close();
		this->state = FINISHED;
	}
}

void	Request::parseRequestBodyWithEncoding(const char* chunk)
{
	this->buffer += chunk;

	while (this->chunkState != LASTCHUNK)
	{
		if (this->chunkState == CHUNKSIZE)
		{
			if (!this->isChunkSizeComplete())
				return ;
			this->calculateChunkSize(); // CHUNKDATA || LASTCHUNK
		}
		if (this->chunkState == CHUNKDATA)
		{
			if (this->buffer.size() < this->chunkSize + 2)
				return ;
			this->parseChunkData(); // CHUNKSIZE || LASTCHUNK
		}
	}
	fileStream.close();
	this->state = FINISHED;
}

bool	Request::isChunkSizeComplete(void)
{
	this->pos = this->buffer.find("\r\n");

	if (this->pos != std::string::npos)
		return (true);
	return (false);
}

void	Request::calculateChunkSize(void)
{
	std::stringstream	strStream;

	strStream << this->buffer.substr(0, this->pos);
	strStream >> std::hex >> this->chunkSize;

	this->buffer.erase(0, this->pos + 2);
	if (this->chunkSize == 0)
		this->chunkState = LASTCHUNK;
	else
		this->chunkState = CHUNKDATA;
}

void	Request::parseChunkData(void)
{
	fileStream << this->buffer.substr(0, this->chunkSize);
	this->ActualContentLen += this->chunkSize;

	if (this->buffer.compare(this->chunkSize, 2, "\r\n") != 0)
	{
		this->ActualContentLen = this->contentLen + 1;
		this->chunkState = LASTCHUNK;
		return ;
	}
	this->buffer.erase(0, this->chunkSize + 2);
	this->chunkState = CHUNKSIZE;
}

const char*	Request::setState(State state)
{
	this->state = state;
	return (NULL);
}

void	Request::setFileName(void)
{
	static unsigned int	count = 0;
	unsigned int		nb = ++count;
	const size_t		sz = this->fileName.size();

	for (; nb != 0; nb = nb / 10)
	    this->fileName.insert(sz, 1, nb % 10 + '0');
}

size_t	Request::stringToNumber(const std::string& str)
{
	size_t	num = 0;

	for (size_t i = 0; i < str.size(); i++)
	{
		num *= 10;
		num = num + (str[i] - '0');
	}
	return (num);
}
