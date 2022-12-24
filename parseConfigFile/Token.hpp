
#ifndef TOKEN_HPP
# define TOKEN_HPP

# include <string>

enum TokenType
{
	TOKEN_WORD,
	TOKEN_LPAREN,
	TOKEN_RPAREN,
	TOKEN_EOL,
	TOKEN_EOF
};

class Token
{
public:
	TokenType	type;
	std::string	value;

public:
	// Constructors && Destructor
	Token(void);
	Token(const TokenType& tp, const std::string& val);
	Token(const Token& src);
	~Token(void);

	// Copy assignment operator
	Token&	operator=(const Token& rhs);
};

#endif
