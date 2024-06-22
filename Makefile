NAME = webserv
UNAME = $(shell uname -s)

CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98

RM = rm -f

SRC_NAME = $(shell find ./src -iname "*.cpp")
OBJ_NAME = $(SRC_NAME:.cpp=.o)

HEADERS = -I ./include


# Colors
GREEN = \033[32m
YELLOW = \033[33m
RESET = \033[0m


# Main commands
all: $(NAME)

clean:
	@echo "$(YELLOW)Cleaning up...$(RESET)"
	@$(RM) $(OBJ_NAME)
	@echo "$(GREEN)Done.$(RESET)"

fclean: clean
	@$(RM) $(NAME)
	@$(RM) $(NAME).dSYM

re: fclean all


# Sub commands
%.o: %.cpp
	@$(CC) $(CFLAGS) -c $< -o $@ $(HEADERS)

dev:
	@$(CC) $(CFLAGS) -g3 -o $(NAME) $(SRC_NAME) $(HEADERS)

$(NAME): $(OBJ_NAME)
	@echo "$(YELLOW)Building $@...$(RESET)"
	@$(CC) $(CFLAGS) -o $(NAME) $(OBJ_NAME)
	@echo "$(GREEN)Done.$(RESET)"


# Custom commands
show:
		@printf "UNAME		: $(UNAME)\n"
		@printf "NAME  		: $(NAME)\n"
		@printf "CC		: $(CC)\n"
		@printf "CFLAGS		: $(CFLAGS)\n"
		@printf "SRC		: $(SRC_NAME)\n"
		@printf "OBJ		: $(OBJ_NAME)\n"

# Phony
.PHONY: all clean fclean re