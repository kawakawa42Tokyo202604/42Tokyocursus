#include "push_swap.h"

int	handle_x(va_list args, int fd)
{
	int				count;
	unsigned int	num;

	num = va_arg(args, unsigned int);
	count = ft_putnbr_base(num, "0123456789abcdef", fd);
	return (count);
}

int	handle_big_x(va_list args, int fd)
{
	int				count;
	unsigned int	num;

	num = va_arg(args, unsigned int);
	count = ft_putnbr_base(num, "0123456789ABCDEF", fd);
	return (count);
}
