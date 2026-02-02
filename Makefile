NAME = teapot
DBG_NAME = teapot-debug

SRC_DIR = ./src/
CONFIG_SRC_DIR = ./src/config/
SERVER_SRC_DIR = ./src/server/
UTILS_SRC_DIR = ./src/utils/

OBJ_DIR = ./build/

INC_DIRS = ./include/config ./include/server ./include/utils
INC_FLAGS = $(addprefix -I,$(INC_DIRS))

SRC_FILES =	$(SRC_DIR)main.cpp \
			$(CONFIG_SRC_DIR)Cgi.cpp \
			$(CONFIG_SRC_DIR)Config.cpp \
			$(CONFIG_SRC_DIR)Delete.cpp \
			$(CONFIG_SRC_DIR)Errors_ReturnMsgs.cpp \
			$(CONFIG_SRC_DIR)Get.cpp \
			$(CONFIG_SRC_DIR)Post.cpp \
			$(CONFIG_SRC_DIR)RequestData.cpp \
			$(CONFIG_SRC_DIR)RouteInfo.cpp \
			$(CONFIG_SRC_DIR)Router.cpp \
			$(CONFIG_SRC_DIR)ServerData.cpp \
			$(SERVER_SRC_DIR)Client.cpp \
			$(SERVER_SRC_DIR)Cookie.cpp \
			$(SERVER_SRC_DIR)CookieJar.cpp \
			$(SERVER_SRC_DIR)HexColorCode.cpp \
			$(SERVER_SRC_DIR)Listener.cpp \
			$(SERVER_SRC_DIR)Request.cpp \
			$(SERVER_SRC_DIR)Response.cpp \
			$(SERVER_SRC_DIR)Server.cpp \
			$(SERVER_SRC_DIR)Socket.cpp \
			$(UTILS_SRC_DIR)Filesystem.cpp \
			$(UTILS_SRC_DIR)Helper.cpp \
			$(UTILS_SRC_DIR)Log.cpp

OBJ_FILES = $(patsubst $(SRC_DIR)%.cpp,$(OBJ_DIR)%.o,$(SRC_FILES))
DBG_OBJ_FILES = $(patsubst $(SRC_DIR)%.cpp,$(OBJ_DIR)%.debug.o,$(SRC_FILES))

REMOVE = rm -f
REMOVE_DIR = rm -rf

CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98 -O2
DBG_CFLAGS = -g -O0

all: $(NAME)

$(NAME): $(OBJ_FILES)
	@echo "Compiling/Linking"
	@$(CC) $(CFLAGS) $(OBJ_FILES) -o $(NAME)
	@echo "Ready"

debug: $(DBG_NAME)

$(DBG_NAME): $(DBG_OBJ_FILES)
	@echo "Compiling/Linking with debug flags"
	@$(CC) $(CFLAGS) $(DBG_CFLAGS) $(DBG_OBJ_FILES) -o $(DBG_NAME)
	@echo "Debug executable ready"

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR) $(OBJ_DIR)config $(OBJ_DIR)server $(OBJ_DIR)utils

$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp | $(OBJ_DIR)
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) $(INC_FLAGS) -c $< -o $@

$(OBJ_DIR)%.debug.o: $(SRC_DIR)%.cpp | $(OBJ_DIR)
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) $(DBG_CFLAGS) $(INC_FLAGS) -c $< -o $@

clean:
	@echo "Removing .o files"
	@$(REMOVE_DIR) $(OBJ_DIR)

fclean:	clean
	@echo "Removing executable"
	@$(REMOVE) $(NAME) $(DBG_NAME)

re:	fclean all

.PHONY: all clean fclean re debug
