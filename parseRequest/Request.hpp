
#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <iostream>
# include <map>
# include <string>
# include <limits>
# include <fstream>
# include <sstream>

enum State      { STARTED      , FINISHED               };
enum Flag       { HEADERS      , BODYCHUNKED, BODY      };
enum ResFlag    { HEADERNOTSENT, HEADERSENT             };
enum ResState   { BODYNOTSENT  , BODYSENT               };
enum ChunkState { CHUNKSIZE    , CHUNKDATA  , LASTCHUNK };

class Request
{
/*DATA MEMBERS*/
public:
	std::map<std::string, std::string>	headers;  // To store request headers
	State								state;    // state of the object
	std::string							fileName; // To store request body
	int									fd;       // fd 
	// Response states and flags
	ResFlag								resFlag;
	ResState							resState;
	int									fdBody;
	size_t								contentLen;
	size_t								ActualContentLen;
private:
	std::ofstream	fileStream;
	Flag			flag;
	ChunkState		chunkState;
	size_t			chunkSize;
	size_t			pos;
	std::string		buffer;         // To collect request headers
	std::string		temp;

/*MEMBER FUNCTIONS*/
public:
	// Constructors && Destructor
	Request(void);
	Request(const int& fd);
	Request(const Request& src);
	~Request(void);

	// Copy assignment operator
	Request&	operator=(const Request& rhs);

	// ENTRY POINT member function
	void	operator()(const char* chunk);
	void	clear(void);
private:
	// parse request HEADERS
	bool	isRequestHeadersComplete(const char* chunk);
	void	parseRequestHeaders(void);
	void	parseRequestLine(const size_t lPos);
	void	parseRequestHeader(const size_t fPos, const size_t lPos);

	// parse request BODY
	void	parseRequestBodyWithoutEncoding(const std::string& chunk);
	void	parseRequestBodyWithEncoding(const char* chunk);
	bool	isChunkSizeComplete(void);
	void	calculateChunkSize(void);
	void	parseChunkData(void);

	const char*	checkAndSetFlags(void);
	const char*	setState(State state);
	void		setFileName(void);
	size_t		stringToNumber(const std::string& str);
};

#endif
