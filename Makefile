
NAME = webserv

SRCS =  main.cpp  ./socket/socket.cpp \
	   ./parseConfigFile/Parser.cpp ./parseConfigFile/Lexer.cpp ./parseConfigFile/Token.cpp \
	   ./parseConfigFile/ServerData.cpp ./parseConfigFile/parseConfigFile.cpp parseRequest/Request.cpp \
	   ./handleResponse/response.cpp

HEADERS = ./socket/socket.hpp \
		  ./parseConfigFile/Parser.hpp ./parseConfigFile/Lexer.hpp ./parseConfigFile/Token.hpp \
		  ./parseConfigFile/Exceptions.hpp ./parseConfigFile/ServerData.hpp \
		  ./parseConfigFile/parseConfigFile.hpp parseRequest/Request.hpp \
		  ./handleResponse/response.hpp 

COMPILER = c++

FLAGS = -std=c++98 -Wall -Wextra -Werror

all: $(NAME)

$(NAME): $(SRCS) $(HEADERS)
	$(COMPILER) $(FLAGS) $(SRCS) -o $(NAME)

clean:
	rm -rf $(NAME)

fclean: clean

re: fclean all

.PHONY: all clean fclean re
