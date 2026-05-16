#include "ft_printf.h"

int	ft_printf(const char *format, ...)
{
	va_list	args;
	int		count;
	int		temp;

	if (!format)
		return (-1);
	count = 0;
	va_start(args, format);
	while (*format)
	{
		if (*format == '%')
		{
			temp = handler(*(format + 1), args);
			if (temp == -1 || count == -1)
				return (-1);
			count += temp;
			++format;
		}
		else
			count = write_error(count, write(1, format, 1));
		++format;
	}
	va_end(args);
	return (count);
}

int	handler(char c, va_list args)
{
	if (c == 'c')
		return (handle_c(args));
	else if (c == 's')
		return (handle_s(args));
	else if (c == 'p')
		return (handle_p(args));
	else if (c == 'd')
		return (handle_d(args));
	else if (c == 'i')
		return (handle_i(args));
	else if (c == 'u')
		return (handle_u(args));
	else if (c == 'x')
		return (handle_little_x(args));
	else if (c == 'X')
		return (handle_big_x(args));
	else if (c == '%')
		return (write_error(0, write(1, "%", 1)));
	return (-1);
}

int	write_error(int count, int value)
{
	if (value == -1 || count == -1)
		return (-1);
	return (count + value);
}
