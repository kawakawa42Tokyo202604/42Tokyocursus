char	*ft_strcat(char *dest, char *src)
{
	unsigned int	i;
	unsigned int	j;
	char			*begin;

	begin = dest;
	i = 0;
	while (dest[i] != '\0')
	{
		++i;
	}
	j = 0;
	while (src[j] != '\0')
	{
		dest[i + j] = src[j];
		++j;
	}
	dest[i + j] = '\0';
	return (begin);
}

// #include <stdio.h>
// #include <string.h>

// int	main(void)
// {
// 	char	s1[30] = "abcdefgh";
// 	char	s2[10] = "abcdjklo";

// 	printf("%s\n", ft_strcat(s1, s2));
// 	printf("%s\n", strcat(s1, s2));
// 	return (0);
// }
