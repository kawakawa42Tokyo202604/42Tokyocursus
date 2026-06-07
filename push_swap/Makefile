NAME            = push_swap
BONUS_NAME      = checker

CC              = cc
CFLAGS          = -Wall -Wextra -Werror

COMMON_SRCS     = parsing.c \
				  parsing_utils.c\
                  stack_init.c \
                  ft_atoi.c \
                  ft_split.c \
                  ft_bzero.c \
                  ft_memset.c \
                  ft_substr.c \
                  ft_strdup.c \
                  ft_strlcpy.c \
                  ft_strlen.c \
                  ft_lst.c \
                  ft_push.c \
                  ft_rotate.c \
                  ft_reverse_rotate.c \
                  ft_swap.c \
                  ft_dprintf.c \
                  ft_putnbr_base.c \
                  handler.c \
                  handler_2.c\
				  ft_strcmp.c

PUSH_SWAP_SRCS  = push_swap.c \
                  compute_disorder.c \
                  ft_strategy_main.c \
                  ft_strategy_simple.c \
                  ft_strategy_medium.c \
                  ft_strategy_complex.c

BONUS_SRCS      = checker.c \
                  get_next_line.c \
                  get_next_line_utils.c

COMMON_OBJS     = $(COMMON_SRCS:.c=.o)
PUSH_SWAP_OBJS  = $(PUSH_SWAP_SRCS:.c=.o)
BONUS_OBJS      = $(BONUS_SRCS:.c=.o)

all: $(NAME)

$(NAME): $(COMMON_OBJS) $(PUSH_SWAP_OBJS)
	$(CC) $(CFLAGS) $^ -o $@

bonus: $(BONUS_NAME)

$(BONUS_NAME): $(COMMON_OBJS) $(BONUS_OBJS)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(COMMON_OBJS) $(PUSH_SWAP_OBJS) $(BONUS_OBJS)

fclean: clean
	rm -f $(NAME) $(BONUS_NAME)

re: fclean all

.PHONY: all clean fclean re bonus
