#include <stdlib.h>

char	*ft_strdup(char *src)
{
	char	*str;
	int		i;
	int		j;

	i = 0;
	while (src[i] != '\0')
		++i;
	str = (char *)malloc(sizeof(char) * (i + 1));
	if (str == 0)
	{
		return (0);
	}
	j = -1;
	while (++j < i)
		str[j] = src[j];
	str[j] = '\0';
	return (str);
}

// #include <stdio.h>

// int	main(void)
// {
// 	char	src[] = "ggccjkcdjkd";

// 	printf("%s", ft_strdup(src));
// }
