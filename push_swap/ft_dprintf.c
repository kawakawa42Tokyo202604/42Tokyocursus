#include "push_swap.h"

static int	parse_format(const char **format, va_list args, int fd)
{
	int	count;

	count = 0;
	(*format)++;
	if (**format == 'c')
		count += handle_c(args, fd);
	else if (**format == 's')
		count += handle_s(args, fd);
	else if (**format == 'p')
		count += handle_p(args, fd);
	else if (**format == 'd' || **format == 'i')
		count += handle_di(args, fd);
	else if (**format == 'u')
		count += handle_u(args, fd);
	else if (**format == 'x')
		count += handle_x(args, fd);
	else if (**format == 'X')
		count += handle_big_x(args, fd);
	else if (**format == '%')
		count += write(fd, "%", 1);
	return (count);
}

int	ft_dprintf(int fd, const char *format, ...)
{
	va_list	args;
	int		total_len;

	total_len = 0;
	va_start(args, format);
	while (*format)
	{
		if (*format == '%')
			total_len += parse_format(&format, args, fd);
		else
			total_len += write (fd, format, 1);
		format++;
	}
	va_end(args);
	return (total_len);
}
