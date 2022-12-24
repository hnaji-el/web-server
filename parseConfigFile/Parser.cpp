
#include "Parser.hpp"

/*
 * Constructors && Destructor
 */

Parser::Parser(const std::string& fileName, std::vector<ServerData>& configData)
	: _lexer(fileName), _configData(configData)
{
	this->_curToken = this->_lexer.lexerGetNextToken();
	this->_prevToken = this->_curToken;

	this->fillServerTable();
	this->fillLocationTable();
}

Parser::~Parser(void)
{
}

/*
 * member functions for dispatching
 */

void	Parser::fillServerTable(void)
{
	this->_serverTable["listen"] = &Parser::parserParseListen;
	this->_serverTable["server_name"] = &Parser::parserParseServerName;
	this->_serverTable["error_page"] = &Parser::parserParseErrorPage;
	this->_serverTable["limit_client_body_size"] = &Parser::parserParseLimitSize;
	this->_serverTable["accepted_methods"] = &Parser::parserParseAcceptedMethods;
	this->_serverTable["root"] = &Parser::parserParseRoot;
	this->_serverTable["index"] = &Parser::parserParseIndex;
	this->_serverTable["autoindex"] = &Parser::parserParseAutoindex;
	this->_serverTable["location"] = &Parser::parserParseLocation;
}

void	Parser::fillLocationTable(void)
{
	this->_locationTable["error_page"] = &Parser::parserParseErrorPageLoc;
	this->_locationTable["limit_client_body_size"] = &Parser::parserParseLimitSizeLoc;
	this->_locationTable["accepted_methods"] = &Parser::parserParseAcceptedMethodsLoc;
	this->_locationTable["root"] = &Parser::parserParseRootLoc;
	this->_locationTable["index"] = &Parser::parserParseIndexLoc;
	this->_locationTable["autoindex"] = &Parser::parserParseAutoindexLoc;
}

/*
 * ENTRY POINT member function
 */

void	Parser::parserParse(void)
{
	while (this->_curToken.type != TOKEN_EOF)
	{
		while (this->_curToken.type == TOKEN_EOL)
			this->expectedToken(TOKEN_EOL);
		if (this->_curToken.value == "server")
		{
			this->expectedToken(TOKEN_WORD);
			this->parserParseServer();
		}
		else if (this->_curToken.type != TOKEN_EOF)
			throw SyntaxError(this->_curToken.value);
	}

	if (this->_configData.empty())
		throw Failure("webserv: [ERROR]: configure at least one server block");
	this->isPortDuplicate(this->_configData);
}

/*
 * parse blocks: server && location
 */

void	Parser::parserParseServer(void)
{
	this->_configData.push_back(ServerData());
	this->_numOfCallS.clear();

	this->expectedToken(TOKEN_LPAREN);
	this->expectedToken(TOKEN_EOL);
	while (this->_curToken.type != TOKEN_RPAREN)
	{
		while (this->_curToken.type == TOKEN_EOL)
			this->expectedToken(TOKEN_EOL);
		if (this->_curToken.type == TOKEN_RPAREN)
			break ;

		SerBlockTable::iterator	iter = this->_serverTable.find(this->_curToken.value);
		if (iter == this->_serverTable.end())
			throw SyntaxError(this->_curToken.value);
		this->expectedToken(TOKEN_WORD);
		(this->*(iter->second))(this->_configData[this->_configData.size() - 1]);
	}
	this->expectedToken(TOKEN_RPAREN);
	if (this->_curToken.type != TOKEN_EOL && this->_curToken.type != TOKEN_EOF)
		throw SyntaxError(this->_curToken.value);
	if (this->_curToken.type == TOKEN_EOL)
		this->expectedToken(TOKEN_EOL);

	this->isLocationDuplicate(this->_configData[this->_configData.size() - 1].location);
	this->isRootExist(this->_configData[this->_configData.size() - 1]);
}

void	Parser::parserParseLocation(ServerData& serData)
{
	serData.location.push_back(LocationData(serData));
	this->_numOfCallL.clear();

	this->expectedToken(TOKEN_WORD);
	serData.location[serData.location.size() - 1].pathname = this->_prevToken.value;
	this->expectedToken(TOKEN_LPAREN);
	this->expectedToken(TOKEN_EOL);
	while (this->_curToken.type != TOKEN_RPAREN)
	{
		while (this->_curToken.type == TOKEN_EOL)
			this->expectedToken(TOKEN_EOL);
		if (this->_curToken.type == TOKEN_RPAREN)
			break ;

		LocBlockTable::iterator	iter = this->_locationTable.find(this->_curToken.value);
		if (iter == this->_locationTable.end())
			throw SyntaxError(this->_curToken.value);
		this->expectedToken(TOKEN_WORD);
		(this->*(iter->second))(serData.location[serData.location.size() - 1]);
	}
	this->expectedToken(TOKEN_RPAREN);
	this->expectedToken(TOKEN_EOL);
}

/*
 * Parse directives of Server block
 */

void	Parser::parserParseListen(ServerData& serData)
{
	long		port = 0;

	this->isDirectiveDuplicate(this->_numOfCallS, "listen");

	this->expectedToken(TOKEN_WORD);
	serData.host = this->_prevToken.value;

	this->expectedToken(TOKEN_WORD);
	port = this->checkAndGetNumber(this->_prevToken.value);
	if (port < 1 || port > 65535)
		throw SyntaxError(this->_prevToken.value);
	serData.port = port;

	this->expectedToken(TOKEN_EOL);
}

void	Parser::parserParseServerName(ServerData& serData)
{
	this->isDirectiveDuplicate(this->_numOfCallS, "server_name");

	this->expectedToken(TOKEN_WORD);
	serData.serverName.push_back(this->_prevToken.value);

	while (this->_curToken.type == TOKEN_WORD)
	{
		this->expectedToken(TOKEN_WORD);
		serData.serverName.push_back(this->_prevToken.value);
	}
	this->expectedToken(TOKEN_EOL);

	this->isDuplicate(serData.serverName, "server_name");
}

void	Parser::parserParseErrorPage(ServerData& serData)
{
	long	num = 0;

	this->expectedToken(TOKEN_WORD);
	num = this->checkAndGetNumber(this->_prevToken.value);
	if (num < 100 || num > 599)
		throw SyntaxError(this->_prevToken.value);
	this->expectedToken(TOKEN_WORD);
	if (serData.statusCode.insert(std::make_pair(num, this->_prevToken.value)).second == false)
		throw Failure("webserv: [ERROR]: `error_page' directive is duplicate");

	this->expectedToken(TOKEN_EOL);
}

void	Parser::parserParseLimitSize(ServerData& serData)
{
	long		num = 0;

	this->isDirectiveDuplicate(this->_numOfCallS, "Limit_client_body_size");

	this->expectedToken(TOKEN_WORD);
	num = this->checkAndGetNumber(this->_prevToken.value);
	if (num == -1)
		throw SyntaxError(this->_prevToken.value);
	serData.limitSize = num;
	
	this->expectedToken(TOKEN_EOL);
}

void	Parser::checkAndSetMethods(std::vector<std::string>& vec, const std::string& str)
{
	if (str != "GET" && str != "POST" && str != "DELETE")
		throw SyntaxError(str);
	vec.push_back(str);
}

void	Parser::parserParseAcceptedMethods(ServerData& serData)
{
	this->isDirectiveDuplicate(this->_numOfCallS, "accepted_methods");

	this->expectedToken(TOKEN_WORD);
	this->checkAndSetMethods(serData.acceptedMeths, this->_prevToken.value);
	for (size_t i = 0; i < 2 && this->_curToken.type == TOKEN_WORD; i++) {
		this->expectedToken(TOKEN_WORD);
		this->checkAndSetMethods(serData.acceptedMeths, this->_prevToken.value);
	}
	this->expectedToken(TOKEN_EOL);

	this->isDuplicate(serData.acceptedMeths, "accepted_methods");
}

void	Parser::parserParseRoot(ServerData& serData)
{
	this->isDirectiveDuplicate(this->_numOfCallS, "root");

	this->expectedToken(TOKEN_WORD);
	serData.root = this->_prevToken.value;
	this->expectedToken(TOKEN_EOL);
}

void	Parser::parserParseIndex(ServerData& serData)
{
	this->isDirectiveDuplicate(this->_numOfCallS, "index");
	
	this->expectedToken(TOKEN_WORD);
	serData.index.push_back(this->_prevToken.value);

	while (this->_curToken.type == TOKEN_WORD)
	{
		this->expectedToken(TOKEN_WORD);
		serData.index.push_back(this->_prevToken.value);
	}
	this->expectedToken(TOKEN_EOL);

	this->isDuplicate(serData.index, "index");
}

void	Parser::parserParseAutoindex(ServerData& serData)
{
	this->isDirectiveDuplicate(this->_numOfCallS, "autoindex");
	
	this->expectedToken(TOKEN_WORD);
	if (this->_prevToken.value != "on")
		throw SyntaxError(this->_prevToken.value);
	serData.autoindex = true;
	this->expectedToken(TOKEN_EOL);
}

/*
 * Parse directives of Location block
 */

void	Parser::parserParseErrorPageLoc(LocationData& locData)
{
	long	num = 0;

	this->clearStatusCodes(locData);

	this->expectedToken(TOKEN_WORD);
	num = this->checkAndGetNumber(this->_prevToken.value);
	if (num < 100 || num > 599)
		throw SyntaxError(this->_prevToken.value);
	this->expectedToken(TOKEN_WORD);
	if (locData.statusCode.insert(std::make_pair(num, this->_prevToken.value)).second == false)
		throw Failure("webserv: [ERROR]: `error_page' directive is duplicate");

	this->expectedToken(TOKEN_EOL);
}

void	Parser::parserParseLimitSizeLoc(LocationData& locData)
{
	long		num = 0;

	this->isDirectiveDuplicate(this->_numOfCallL, "Limit_client_body_size");

	this->expectedToken(TOKEN_WORD);
	num = this->checkAndGetNumber(this->_prevToken.value);
	if (num == -1)
		throw SyntaxError(this->_prevToken.value);
	locData.limitSize = num;
	
	this->expectedToken(TOKEN_EOL);
}

void	Parser::parserParseAcceptedMethodsLoc(LocationData& locData)
{
	this->isDirectiveDuplicate(this->_numOfCallL, "accepted_methods");
	locData.acceptedMeths.clear();

	this->expectedToken(TOKEN_WORD);
	this->checkAndSetMethods(locData.acceptedMeths, this->_prevToken.value);
	for (size_t i = 0; i < 2 && this->_curToken.type == TOKEN_WORD; i++) {
		this->expectedToken(TOKEN_WORD);
		this->checkAndSetMethods(locData.acceptedMeths, this->_prevToken.value);
	}
	this->expectedToken(TOKEN_EOL);

	this->isDuplicate(locData.acceptedMeths, "accepted_methods");
}

void	Parser::parserParseRootLoc(LocationData& locData)
{
	this->isDirectiveDuplicate(this->_numOfCallL, "root");

	this->expectedToken(TOKEN_WORD);
	locData.root = this->_prevToken.value;
	this->expectedToken(TOKEN_EOL);
}

void	Parser::parserParseIndexLoc(LocationData& locData)
{
	this->isDirectiveDuplicate(this->_numOfCallL, "index");
	locData.index.clear();

	this->expectedToken(TOKEN_WORD);
	locData.index.push_back(this->_prevToken.value);

	while (this->_curToken.type == TOKEN_WORD)
	{
		this->expectedToken(TOKEN_WORD);
		locData.index.push_back(this->_prevToken.value);
	}
	this->expectedToken(TOKEN_EOL);

	this->isDuplicate(locData.index, "index");
}

void	Parser::parserParseAutoindexLoc(LocationData& locData)
{
	this->isDirectiveDuplicate(this->_numOfCallL, "autoindex");

	this->expectedToken(TOKEN_WORD);
	if (this->_prevToken.value != "on")
		throw SyntaxError(this->_prevToken.value);
	locData.autoindex = true;
	this->expectedToken(TOKEN_EOL);
}

/*
 * Helper member functions
 */

void	Parser::expectedToken(TokenType type)
{
	if (this->_curToken.type != type)
		throw SyntaxError(this->_curToken.value);

	this->_prevToken = this->_curToken;
	this->_curToken = this->_lexer.lexerGetNextToken();
}

bool	Parser::isNumber(const std::string& str)
{
	return (str.find_first_not_of("0123456789") == std::string::npos);
}

size_t	Parser::stringToNumber(const std::string& str)
{
	size_t	num = 0;

	for (size_t i = 0; i < str.size(); i++)
	{
		num *= 10;
		num = num + (str[i] - '0');
	}
	return (num);
}

long	Parser::checkAndGetNumber(const std::string& str)
{
	size_t	num = 0;

	if (!this->isNumber(str) || str.size() > 19 ||
		(num = this->stringToNumber(str)) > (size_t)std::numeric_limits<long>::max())
	{
		return (-1);
	}
	return (num);
}

void	Parser::clearStatusCodes(LocationData& locData)
{
	if (locData.flag == false)
	{
		locData.statusCode.clear();
		locData.flag = true;
	}
}

void	Parser::isRootExist(const ServerData& serData)
{
	if (serData.root.empty())
		throw Failure("webserv: [ERROR]: `root' directive doesn't exist in server block");
	for (size_t i = 0; i < serData.location.size(); i++)
	{
		if (serData.location[i].root.empty())
			throw Failure("webserv: [ERROR]: `root' directive doesn't exist in location block");
	}
}

void	Parser::isDirectiveDuplicate(std::set<std::string>& st, const std::string& str)
{
	if (st.insert(str).second == false)
		throw Failure("webserv: [ERROR]: `" + str + "' directive is duplicate");
}

void	Parser::isLocationDuplicate(const std::vector<LocationData>& loc)
{
	std::vector<std::string>	vec(loc.size());

	for (size_t i = 0; i < loc.size(); i++)
		vec[i] = loc[i].pathname;

	this->isDuplicate(vec, "location");
}

void	Parser::isPortDuplicate(const std::vector<ServerData>& ser)
{
	std::vector<unsigned short>	vec(ser.size());

	for (size_t i = 0; i < ser.size(); i++)
		vec[i] = ser[i].port;

	this->isDuplicate(vec, "listen :port");
}

template<class T>
void	Parser::isDuplicate(const std::vector<T>& vec, const std::string& str)
{
	const std::set<T>	st(vec.begin(), vec.end());

	if (st.size() != vec.size())
		throw Failure("webserv: [ERROR]: `" + str + "' directive is duplicate");
}
