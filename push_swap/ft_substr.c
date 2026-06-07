#include "push_swap.h"

#ifdef TEST
# include <stdio.h>
#endif

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	size_t	s_len;
	size_t	actual_len;
	char	*substr;

	if (!s)
		return (NULL);
	s_len = ft_strlen(s);
	if (start >= s_len)
		return (ft_strdup(""));
	actual_len = s_len - start;
	if (actual_len > len)
		actual_len = len;
	substr = (char *)malloc(sizeof(char) * (actual_len + 1));
	if (!substr)
		return (NULL);
	ft_strlcpy(substr, s + start, actual_len + 1);
	return (substr);
}

#ifdef TEST

int	main(void)
{
	char	*res;

	res = ft_substr("Hello World", 6, 5);
	printf("Test 1 (Normal): [%s] (Expected: [World])\n", res);
	free(res);
	res = ft_substr("42", 5, 2);
	printf("Test 2 (Out of range): [%s] (Expected: [])\n", res);
	free(res);
	res = ft_substr("Tokyo", 2, 10);
	printf("Test 3 (Len too long): [%s] (Expected: [kyo])\n", res);
	free(res);
	res = ft_substr("", 0, 1);
	printf("Test 4 (Empty string): [%s] (Expected: [])\n", res);
	free(res);
	return (0);
}
#endif
