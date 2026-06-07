NAME = bsq
CC = cc
CFLAGS = -Wall -Wextra -Werror

SRCS = ft_atoi.c \
       ft_check_map.c \
       ft_condition.c \
       ft_copy.c \
       ft_free.c \
       ft_get_map.c \
       ft_init.c \
       main.c \
       ft_manage_file.c \
       ft_matrix_size.c \
       ft_putchar.c \
       ft_putstr.c \
       ft_solver.c \
       ft_split.c \
       ft_strdup.c \
       ft_strlen.c

OBJS = $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
