CC = c++
INC = -Iinclude -Iinclude/config -Iinclude/server -Iinclude/utils
CFLAGS = -Wall -Wextra -Werror -std=c++98 $(INC) -g# -fsanitize=address
LDFLAGS =
NAME = webserv
DIR_OBJ = build
SRC = $(shell find src -name '*.cpp')
OBJ = $(patsubst %.cpp, $(DIR_OBJ)/%.o, $(SRC))

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(DIR_OBJ)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -rf $(DIR_OBJ)

fclean: clean
	rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean re
