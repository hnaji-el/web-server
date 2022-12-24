
#ifndef LEXER_HPP
# define LEXER_HPP

# include <iostream>
# include <fstream>
# include <string>
# include <cctype>
# include "Token.hpp"
# include "Exceptions.hpp"

class Lexer
{
private:
	std::ifstream	_fileStream;
	std::string		_line;
	size_t			_curIndex;
	char			_curChar;

public:
	// Constructors && Destructor
	Lexer(const std::string& fileName);
	~Lexer(void);

	// Member functions
	Token	lexerGetNextToken(void);
	void	lexerSkipWhitespaces(void);
	void	lexerAdvance(void);
	Token	getLparenToken(void);
	Token	getRparenToken(void);
	Token	getWordToken(void);
	Token	getEndOfLineToken(void);
};

#endif
