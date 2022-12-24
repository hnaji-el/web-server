
#include "Lexer.hpp"

/*
 * Constructors && Destructor
 */

Lexer::Lexer(const std::string& fileName)
{
	this->_fileStream.open(fileName);
	if (!this->_fileStream)
		throw Failure("webserv: [ERROR]: opening/reading/closing file");
	std::getline(this->_fileStream, this->_line);
	this->_curIndex = 0;
	this->_curChar = this->_line[0];
}

Lexer::~Lexer(void)
{
}

/*
 * Member functions
 */

Token	Lexer::lexerGetNextToken(void)
{
	while (this->_curChar != '\0')
	{
		this->lexerSkipWhitespaces();
		if (this->_curChar == '{')
			return (this->getLparenToken());
		if (this->_curChar == '}')
			return (this->getRparenToken());
		if (this->_curChar != '\0')
			return (this->getWordToken());
	}

	if (this->_fileStream.eof())
		return (Token(TOKEN_EOF, "EOF"));
	return (getEndOfLineToken());
}

void	Lexer::lexerSkipWhitespaces(void)
{
	while (isspace(this->_curChar)) {
		this->lexerAdvance();
	}
}

void	Lexer::lexerAdvance(void)
{
	this->_curIndex += 1;
	this->_curChar = this->_line[this->_curIndex];
}

Token	Lexer::getLparenToken(void)
{
	this->lexerAdvance();
	return (Token(TOKEN_LPAREN, "{"));
}

Token	Lexer::getRparenToken(void)
{
	this->lexerAdvance();
	return (Token(TOKEN_RPAREN, "}"));
}

Token	Lexer::getWordToken(void)
{
	std::string	word;

	while (this->_curChar != '\0' && !isspace(this->_curChar) &&
			this->_curChar != '{' && this->_curChar != '}')
	{
		word += this->_curChar;
		this->lexerAdvance();
	}
	return (Token(TOKEN_WORD, word));
}

Token	Lexer::getEndOfLineToken(void)
{
	std::getline(this->_fileStream, this->_line);
	this->_curIndex = 0;
	this->_curChar = this->_line[0];

	return (Token(TOKEN_EOL, "EOL"));
}
