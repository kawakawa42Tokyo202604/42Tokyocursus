unsigned int	ft_strlcat(char *dest, char *src, unsigned int size)
{
	unsigned int	d_len;
	unsigned int	s_len;
	unsigned int	i;

	d_len = 0;
	while (dest[d_len] != '\0' && d_len < size)
		d_len++;
	s_len = 0;
	while (src[s_len] != '\0')
		s_len++;
	if (size <= d_len)
	{
		return (size + s_len);
	}
	i = 0;
	while (src[i] != '\0' && (d_len + i + 1) < size)
	{
		dest[d_len + i] = src[i];
		i++;
	}
	dest[d_len + i] = '\0';
	return (d_len + s_len);
}

// #include <stdio.h>
// #include <string.h>

// int	main(void)
// {
// 	char	s1[30] = "abcdefgh";
// 	char	s2[10] = "abcdjklo";

// 	printf("%u\n", ft_strlcat(s1, s2, 15));
// 	return (0);
// }
