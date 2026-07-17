NAME        = codexion
CC          = cc
CFLAGS      = -Wall -Wextra -Werror -pthread

OBJ_DIR     = obj

SRC_FILES   = main.c init.c heap_ops.c simulation.c utils.c simulation2.c

OBJS        = $(addprefix $(OBJ_DIR)/, $(SRC_FILES:.c=.o))

IFLAGS      = -I.

GREEN       = \033[0;32m
YELLOW      = \033[0;33m
RESET       = \033[0m

all: $(NAME)

$(NAME): $(OBJS)
	@echo "$(YELLOW)Linking $(NAME)...$(RESET)"
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)
	@echo "$(GREEN)$(NAME) has been successfully built!$(RESET)"

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(IFLAGS) -c $< -o $@

clean:
	@echo "$(YELLOW)Cleaning object files...$(RESET)"
	rm -rf $(OBJ_DIR)
	@echo "$(GREEN)Object files cleaned.$(RESET)"

fclean: clean
	@echo "$(YELLOW)Removing $(NAME)...$(RESET)"
	rm -f $(NAME)
	@echo "$(GREEN)$(NAME) removed.$(RESET)"

re: fclean all


valgrind: re
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(NAME) 4 410 200 100 100 3 0 fifo

.PHONY: all clean fclean re valgrind
