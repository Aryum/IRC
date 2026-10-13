SRC = main.cpp Server.cpp Client.cpp
OBJ = $(SRC:.cpp=.o)
CXX = c++
RM = rm -f
CXXFLAGS = -Wall -Wextra -Werror -std=c++98
NAME = ircserv

all: $(NAME);

$(NAME) : $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean $(NAME)

.PHONY: all clean fclean re