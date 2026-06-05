#include "push_swap.h"

int	handle_c(va_list args, int fd)
{
	int		count;
	char	c;

	c = va_arg(args, int);
	count = write (fd, &c, 1);
	return (count);
}

int	handle_s(va_list args, int fd)
{
	int		count;
	char	*str;

	count = 0;
	str = va_arg(args, char *);
	if (!str)
		str = "(null)";
	while (str[count])
	{
		if (write (fd, &str[count], 1) == -1)
			return (-1);
		count++;
	}
	return (count);
}

int	handle_p(va_list args, int fd)
{
	unsigned long	res;
	int				count;

	count = 0;
	res = (unsigned long)va_arg(args, void *);
	if (!res)
		return (write (fd, "(nil)", 5));
	count += write (fd, "0x", 2);
	count += ft_putnbr_base(res, "0123456789abcdef", fd);
	return (count);
}

int	handle_di(va_list args, int fd)
{
	int				n;
	unsigned int	num;
	int				count;

	count = 0;
	n = va_arg(args, int);
	if (n < 0)
	{
		count += write(fd, "-", 1);
		num = (unsigned int)(-n);
	}
	else
		num = (unsigned int)n;
	count += ft_putnbr_base(num, "0123456789", fd);
	return (count);
}

int	handle_u(va_list args, int fd)
{
	unsigned int	n;
	int				count;

	n = va_arg(args, unsigned int);
	count = ft_putnbr_base(n, "0123456789", fd);
	return (count);
}
