char	*ft_strncat(char *dest, char *src, unsigned int nb)
{
	unsigned int	i;
	unsigned int	j;
	char			*begin;

	begin = dest;
	if (nb == 0)
		return (begin);
	i = 0;
	while (dest[i] != '\0')
	{
		++i;
	}
	j = 0;
	while (src[j] != '\0' && j < nb)
	{
		dest[i] = src[j];
		++i;
		++j;
	}
	dest[i] = '\0';
	return (begin);
}

// #include <stdio.h>
// #include <string.h>

// int	main(void)
// {
// 	char	s1[30] = "abcdefgh";
// 	char	s2[10] = "abcdjklo";

// 	printf("%s\n", ft_strncat(s1, s2, 6));
// 	printf("%s\n", strncat(s1, s2, 6));
// 	return (0);
// }
