//#include <unistd.h>

void	ft_judge(char c, int *up, int *down, int *start);
char	*ft_strcapitalize(char *str);

char	*ft_strcapitalize(char *str)
{
	int		up;
	int		down;
	int		i;
	int		start;
	char	*begin;

	begin = &str[0];
	i = 0;
	start = 1;
	while (str[i] != '\0')
	{
		ft_judge(str[i], &up, &down, &start);
		if (start == 1 && (up || down || ('0' <= str[i] && str[i] <= '9')))
		{
			if (down)
				str[i] = str[i] - 'a' + 'A';
			start = 0;
		}
		else if (start == 0 && up)
			str[i] += 32;
		++i;
	}
	return (begin);
}

void	ft_judge(char c, int *up, int *down, int *start)
{
	*up = 0;
	*down = 0;
	if ('a' <= c && c <= 'z')
	{
		*down = 1;
	}
	else if ('A' <= c && c <= 'Z')
	{
		*up = 1;
	}
	else if (!('0' <= c && c <= '9'))
	{
		*start = 1;
	}
}

// int	main(void)
// {
// 	char	str[30] = "  a";
// 	char	*array;
// 	int		i;

// 	i = 0;
// 	array = ft_strcapitalize(str);
// 	while (array[i] != '\0')
// 	{
// 		write(1, &array[i], 1);
// 		++i;
// 	}
// }
