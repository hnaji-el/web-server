
NAME = webserv

SRCS = main.cpp ./utils.cpp ./request_data.cpp ./server.cpp \
	   ./parseConfigFile/Parser.cpp ./parseConfigFile/Lexer.cpp ./parseConfigFile/Token.cpp \
	   ./parseConfigFile/ServerData.cpp ./parseConfigFile/parseConfigFile.cpp

HEADERS = ./parseConfigFile/Parser.hpp ./parseConfigFile/Lexer.hpp ./parseConfigFile/Token.hpp \
		  ./parseConfigFile/Exceptions.hpp ./parseConfigFile/ServerData.hpp \
		  ./parseConfigFile/parseConfigFile.hpp

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
