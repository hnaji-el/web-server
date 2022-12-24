
#ifndef PARSER_HPP
# define PARSER_HPP

# include <iostream>
# include <string>
# include <map>
# include <set>
# include <limits>
# include "Lexer.hpp"
# include "Token.hpp"
# include "ServerData.hpp"
# include "Exceptions.hpp"

class Parser
{
/* MEMBER TYPES */
private:
	typedef std::map<std::string, void (Parser::*)(ServerData&)>	SerBlockTable;
	typedef std::map<std::string, void (Parser::*)(LocationData&)>	LocBlockTable;

/* DATA MEMBERS */
private:
	Lexer	_lexer;
	Token	_curToken;
	Token	_prevToken;
	// AST
	std::vector<ServerData>&	_configData;
	// DISPATCH Tables
	SerBlockTable	_serverTable;
	LocBlockTable	_locationTable;
	// sets for directive duplications
	std::set<std::string>	_numOfCallS;
	std::set<std::string>	_numOfCallL;

/* MEMBER FUNCTIONS */
public:
	// Constructors && Destructor
	Parser(const std::string& fileName, std::vector<ServerData>& configData);
	~Parser(void);

	// ENTRY POINT member function
	void	parserParse(void);

	// parse blocks: server && location
	void	parserParseServer(void);
	void	parserParseLocation(ServerData& serData);

	// parse directives of Server block
	void	parserParseListen(ServerData& serData);
	void	parserParseServerName(ServerData& serData);
	void	parserParseErrorPage(ServerData& serData);
	void	parserParseLimitSize(ServerData& serData);
	void	parserParseAcceptedMethods(ServerData& serData);
	void	parserParseRoot(ServerData& serData);
	void	parserParseIndex(ServerData& serData);
	void	parserParseAutoindex(ServerData& serData);

	// parse directives of Location block
	void	parserParseErrorPageLoc(LocationData& locData);
	void	parserParseLimitSizeLoc(LocationData& locData);
	void	parserParseAcceptedMethodsLoc(LocationData& locData);
	void	parserParseRootLoc(LocationData& locData);
	void	parserParseIndexLoc(LocationData& locData);
	void	parserParseAutoindexLoc(LocationData& locData);

	// HELPER member function
	void	expectedToken(TokenType type);
	bool	isNumber(const std::string& str);
	size_t	stringToNumber(const std::string& str);
	long	checkAndGetNumber(const std::string& str);
	void	checkAndSetMethods(std::vector<std::string>& vec, const std::string& str);
	void	clearStatusCodes(LocationData& locData);

	void	isRootExist(const ServerData& serData);
	void	isDirectiveDuplicate(std::set<std::string>& st, const std::string& str);
	void	isLocationDuplicate(const std::vector<LocationData>& loc);
	void	isPortDuplicate(const std::vector<ServerData>& ser);

	template<class T>
		void	isDuplicate(const std::vector<T>& vec, const std::string& str);

	// fill DISPATCH Tables
	void	fillServerTable(void);
	void	fillLocationTable(void);
};

#endif
