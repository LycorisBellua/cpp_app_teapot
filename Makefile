CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98 -Iinclude -g# -fsanitize=address
LDFLAGS =
NAME = webserv
DIR_OBJ = build
# TODO: SRC files are to be hardcoded, not fetched automatically
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
