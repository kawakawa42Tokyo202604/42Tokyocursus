//#include <unistd.h>

int	ft_strlen(char *str)
{
	int	i;

	i = 0;
	while (*str != '\0')
	{
		++str;
		++i;
	}
	return (i);
}

// void	ft_putnbr(int nb)
// {
// 	char	h[20];
// 	int		i;

// 	i = 0;
// 	if (nb == -2147483648)
// 	{
// 		write(1, "-2147483648", 11);
// 		return ;
// 	}
// 	if (nb < 0)
// 	{
// 		write(1, "-", 1);
// 		nb = -nb;
// 	}
// 	while (nb >= 10)
// 	{
// 		h[i++] = (nb % 10) + '0';
// 		nb = nb / 10;
// 	}
// 	h[i] = nb + '0';
// 	while (i >= 0)
// 	{
// 		write(1, &h[i], 1);
// 		i--;
// 	}
// }

// int	main(void)
// {
// 	char	*str;

// 	str = "abc";
// 	ft_putnbr(ft_strlen(str));
// }
