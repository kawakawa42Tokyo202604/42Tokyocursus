# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: takawaka <takawaka@student.42tokyo.jp>     +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/04/06 14:52:10 by takawaka          #+#    #+#              #
#    Updated: 2026/04/06 14:53:11 by takawaka         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME    = libft.a
CC      = cc
CFLAGS  = -Wall -Wextra -Werror

INCDIR  = includes
SRCDIR  = srcs

SRCS    = $(SRCDIR)/ft_putchar.c \
          $(SRCDIR)/ft_putstr.c \
          $(SRCDIR)/ft_strcmp.c \
          $(SRCDIR)/ft_strlen.c \
          $(SRCDIR)/ft_swap.c

OBJS    = $(SRCS:.c=.o)


all: $(NAME)

$(NAME): $(OBJS)
	ar rc $(NAME) $(OBJS)
	ranlib $(NAME)

.c.o:
	$(CC) $(CFLAGS) -I $(INCDIR) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
