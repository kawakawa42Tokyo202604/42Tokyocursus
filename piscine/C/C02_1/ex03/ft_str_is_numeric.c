//#include <unistd.h>

int	ft_str_is_numeric(char *str)
{
	while (*str != '\0')
	{
		if (!('0' <= *str && *str <= '9'))
		{
			return (0);
		}
		++str;
	}
	return (1);
}

// int	main(void)
// {
// 	char	str[30] = "bananan";
// 	int		i;
// 	char	c;

// 	i = ft_str_is_alpha(str);
// 	c = i + '0';
// 	write(1, &c, 1);
// }
