#include <unistd.h>

int	duplicate_check(char *base);
int	ft_atoi_base(char *str, char *base);
int	get_base_num(char c, char *base);

int	ft_atoi_base(char *str, char *base)
{
	int	res;
	int	sign;
	int	base_len;

	res = 0;
	sign = 1;
	if (duplicate_check(base))
		return (0);
	base_len = 0;
	while (base[base_len])
		base_len++;
	while ((*str >= 9 && *str <= 13) || *str == 32)
		str++;
	while (*str == '-' || *str == '+')
	{
		if (*str == '-')
			sign *= -1;
		str++;
	}
	while (get_base_num(*str, base) != -1)
	{
		res = res * base_len + get_base_num(*str, base);
		str++;
	}
	return (res * sign);
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
		if (base[i] == '-' || base[i] == '+' || (base[i] >= 9 && base[i] <= 13)
			|| base[i] == 32)
			return (1);
		c = (unsigned char)base[i];
		if (check[c] == 1)
			return (1);
		check[c] = 1;
		++i;
	}
	if (i < 2)
		return (1);
	return (0);
}

int	get_base_num(char c, char *base)
{
	int	i;

	i = 0;
	while (base[i])
	{
		if (base[i] == c)
			return (i);
		++i;
	}
	return (-1);
}

// #include <stdio.h>

// int	main(void)
// {
// 	char	str[] = "erweyytii";
// 	char	base[] = "qwertyuiop";

// 	printf("%d", ft_atoi_base(str, base));
// 	return (0);
// }
