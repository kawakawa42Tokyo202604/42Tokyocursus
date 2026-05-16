#include "ft_printf.h"

int	handle_u(va_list args)
{
	unsigned int	nbr;

	nbr = va_arg(args, unsigned int);
	return (put_nbr_base(nbr, "0123456789"));
}

int	handle_little_x(va_list args)
{
	unsigned int	nbr;

	nbr = va_arg(args, unsigned int);
	return (put_nbr_base(nbr, "0123456789abcdef"));
}

int	handle_big_x(va_list args)
{
	unsigned int	nbr;

	nbr = va_arg(args, unsigned int);
	return (put_nbr_base(nbr, "0123456789ABCDEF"));
}

int	put_nbr_base(unsigned long nbr, char *base)
{
	unsigned long	base_len;
	char			res[36];
	int				i;
	int				len;
	int				temp;

	base_len = ft_strlen(base);
	len = get_nbr_len(nbr, base_len);
	i = len - 1;
	if (nbr == 0)
		res[0] = base[0];
	while (nbr > 0)
	{
		res[i] = base[nbr % base_len];
		nbr = nbr / base_len;
		--i;
	}
	temp = write(1, res, len);
	if (temp == -1)
		return (-1);
	return (temp);
}

int	get_nbr_len(unsigned long nbr, unsigned long base_len)
{
	int	len;

	len = 1;
	while (nbr >= base_len)
	{
		nbr = nbr / base_len;
		++len;
	}
	return (len);
}
