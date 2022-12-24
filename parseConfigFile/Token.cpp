
#include "Token.hpp"

/*
 * Constructors && Destructor
 */

Token::Token(void)
	: type(), value()
{
}

Token::Token(const TokenType& tp, const std::string& val)
	: type(tp), value(val)
{
}

Token::Token(const Token& src)
	: type(src.type), value(src.value)
{
}

Token::~Token(void)
{
}

/*
 * Copy assignment operator
 */

Token&	Token::operator=(const Token& rhs)
{
	if (this != &rhs)
	{
		this->type = rhs.type;
		this->value = rhs.value;
	}

	return (*this);
}
