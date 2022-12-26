
#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <iostream>
# include <map>
# include <string>

enum State
{
	STARTED,
	FINISHED
};

enum Flag
{
	HEADERS,
	BODY
};

class Request
{
public:
	std::map<std::string, std::string>	headers;  // To store request headers
	State								state;    // state of the object
	std::string							fileName; // To store request body
	int									fd;       // fd 
	std::string							buffer;   // To collect request headers
	size_t								pos;      // position of CRLFCRLF
	Flag								flag;

public:
	Request(void);
	Request(const int fd);
	~Request(void);

	// ENTRY POINT member function
	void	operator()(const char* buff);
private:
	// Helper member functions
	void	parseRequest(void);
	void	parseRequestHeaders(void);
	void	parseRequestLine(const size_t fPos, const size_t lPos);
	void	parseRequestHeader(const size_t fPos, const size_t lPos);
	void	parseRequestBody(void);

	bool	collectRequestHeaders(const char* buff);
};

#endif
