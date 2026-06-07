#include <unistd.h>

void	ft_putstr(char *str)
{
	unsigned int	i;

	if (!str)
		return ;
	i = 0;
	while (str[i] != '\0')
	{
		++i;
	}
	write(1, str, i);
}

// #include <stdio.h>
// #include <string.h>

// int	main(void)
// {
// 	char	str[] = "haveuhv";

// 	ft_putstr(str);
// }
