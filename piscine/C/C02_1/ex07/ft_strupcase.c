// #include <unistd.h>

char	*ft_strupcase(char *str)
{
	char	*start;

	start = str;
	while (*str != '\0')
	{
		if ('a' <= *str && *str <= 'z')
		{
			*str = *str - 32;
		}
		++str;
	}
	return (start);
}

// int	main(void)
// {
// 	char	str[30] = "toumaHHdgsyuA";

// 	*ft_strupcase(str);
// }
