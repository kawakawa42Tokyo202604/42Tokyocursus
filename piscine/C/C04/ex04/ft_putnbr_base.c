#include <unistd.h>

int		duplicate_check(char *base);
void	ft_putnbr_base(int nbr, char *base);
void	print_nbr(int i, char *buf);

void	ft_putnbr_base(int nbr, char *base)
{
	char	buf[33];
	int		base_len;
	int		i;
	long	n;

	if (duplicate_check(base))
		return ;
	base_len = 0;
	while (base[base_len])
		++base_len;
	n = nbr;
	if (n < 0)
	{
		write(1, "-", 1);
		n = -n;
	}
	i = 0;
	if (n == 0)
		buf[i++] = base[0];
	while (n > 0)
	{
		buf[i++] = base[n % base_len];
		n = n / base_len;
	}
	print_nbr(i, &buf[0]);
}

void	print_nbr(int i, char *buf)
{
	while (--i >= 0)
		write(1, &buf[i], 1);
}

int	duplicate_check(char *base)
{
	int				check[256];
	int				i;
	unsigned char	c;

	i = -1;
	while (++i < 256)
		check[i] = 0;
	i = 0;
	while (base[i] != '\0')
	{
		if (base[i] == '-' || base[i] == '+')
			return (1);
		c = (unsigned char)base[i];
		if (check[c] == 1)
			return (1);
		check[c] = 1;
		i++;
	}
	if (i < 2)
		return (1);
	return (0);
}

// int	main(void)
// {
// 	write(1, "10n: ", 5);
// 	ft_putnbr_base(-2147483648, "0123456789");
// 	write(1, "\n", 1);
// 	write(1, "16n: ", 5);
// 	ft_putnbr_base(255, "0123456789ABCDEF");
// 	write(1, "\n", 1);
// 	write(1, "02n: ", 5);
// 	ft_putnbr_base(42, "01");
// 	write(1, "\n", 1);
// 	write(1, "Err: ", 5);
// 	ft_putnbr_base(42, "011");
// 	ft_putnbr_base(42, "0");
// 	ft_putnbr_base(42, "0+1");
// 	write(1, "\n", 1);
// 	return (0);
// }
