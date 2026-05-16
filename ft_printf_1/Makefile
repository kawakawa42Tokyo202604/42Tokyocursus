NAME    = libftprintf.a
CC      = cc
CFLAGS  = -Wall -Wextra -Werror
SRCS    = ft_printf.c ft_handle_1.c ft_handle_2.c ft_strlen.c
OBJS    = $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	ar rcs $(NAME) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
