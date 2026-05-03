#include <stdlib.h>

int		duplicate_check(char *base);
int		get_base_num(char c, char *base);
int		ft_atoi_base(char *nbr, int len, char *base_from);
int		ft_strlen(char *str);
int		get_base_num(char c, char *base);
int		get_res_len(long n, int base_len);
char	*ft_convert_base(char *nbr, char *base_from, char *base_to);
char	*ft_int_to_base(int base, char *base_to, int base_len);

char	*ft_convert_base(char *nbr, char *base_from, char *base_to)
{
	int	base_len_from;
	int	base_len_to;
	int	res;

	if (duplicate_check(base_from) || duplicate_check(base_to))
		return (NULL);
	base_len_from = ft_strlen(base_from);
	base_len_to = ft_strlen(base_to);
	res = ft_atoi_base(nbr, base_len_from, base_from);
	return (ft_int_to_base(res, base_to, base_len_to));
}

int	ft_atoi_base(char *nbr, int base_len, char *base_from)
{
	int	res;
	int	sign;
	int	i;

	res = 0;
	sign = 1;
	i = 0;
	while ((9 <= nbr[i] && nbr[i] <= 13) || nbr[i] == ' ')
		++i;
	while (nbr[i] == '+' || nbr[i] == '-')
	{
		if (nbr[i] == '-')
			sign = sign * (-1);
		++i;
	}
	while (get_base_num(nbr[i], base_from) != -1)
	{
		res = res * base_len + get_base_num(nbr[i], base_from);
		i++;
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
		if (base[i] == '-' || base[i] == '+' || (9 <= base[i] && base[i] <= 13)
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

int	ft_strlen(char *str)
{
	int	i;

	i = 0;
	while (str[i] != '\0')
	{
		++i;
	}
	return (i);
}

// #include <stdio.h>

// int	main(void)
// {
// 	char	*res;

// 	res = ft_convert_base("10101001", "01", "0123456789");
// 	printf("Test 1 (169): %s\n", res);
// 	free(res);

// 	res = ft_convert_base("-42", "0123456789", "0123456789ABCDEF");
// 	printf("Test 2 (-2A): %s\n", res);
// 	free(res);

// 	res = ft_convert_base("0", "0123456789", "01");
// 	printf("Test 3 (0): %s\n", res);
// 	free(res);

// 	return (0);
// }
