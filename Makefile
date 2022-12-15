NAME = webserv

SRC = ./main.cpp 


FLAG =  -Wall -Wextra -Werror -std=c++98
CC = c++ 
all: $(NAME)

$(NAME): $(SRC) 
	$(CC)  $(SRC) -o $(NAME)


clean:
	echo -n

fclean: clean
	rm -f $(NAME)

re: fclean all