
#ifndef EXCEPTIONS_HPP
# define EXCEPTIONS_HPP

# include <exception>
# include <string>

class Failure : std::exception
{
private:
	const std::string	_msg;

public:
	Failure(const std::string& msg) : _msg(msg) { }
	virtual ~Failure(void) throw() { }

	virtual const char*	what() const throw()
	{
		return (this->_msg.c_str());
	}
};

class SyntaxError : std::exception
{
private:
	std::string	_msg;

public:
	SyntaxError(const std::string& tokenValue)
	{
		this->_msg = "webserv: [ERROR]: syntax error near unexpected token `";
		this->_msg += tokenValue;
		this->_msg += "'";
	}
	virtual ~SyntaxError(void) throw() { }

	virtual const char*	what() const throw()
	{
		return (this->_msg.c_str());
	}
};

#endif
