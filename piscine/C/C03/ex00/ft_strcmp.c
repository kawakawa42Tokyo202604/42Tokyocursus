int	ft_strcmp(char *s1, char *s2)
{
	unsigned int	i;

	i = 0;
	while (s1[i] != '\0' && s2[i] != '\0' && s1[i] == s2[i])
	{
		++i;
	}
	return ((unsigned char)s1[i] - (unsigned char)s2[i]);
}

// #include <stdio.h>
// #include <string.h>

// int	main(void)
// {
// 	char	s1[10] = "abcdefgh";
// 	char	s2[10] = "abcdjklo";

// 	printf("%d\n", ft_strcmp(s1, s2));
// 	printf("%d\n", strcmp(s1, s2));
// 	return (0);
// }
