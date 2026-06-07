#include "push_swap.h"

char	*ft_strchr(const char *s, int c)
{
	size_t	i;

	if (!s)
		return (NULL);
	if (c == '\0')
		return ((char *)&s[ft_strlen(s)]);
	i = 0;
	while (s[i])
	{
		if (s[i] == (char)c)
			return ((char *)&s[i]);
		i++;
	}
	return (NULL);
}

char	*ft_strjoin(char *s1, const char *s2)
{
	char	*res;
	size_t	len1;
	size_t	len2;

	if (!s2)
		return (NULL);
	len1 = 0;
	if (s1)
		len1 = ft_strlen(s1);
	len2 = ft_strlen(s2);
	res = (char *)malloc(sizeof(char) * (len1 + len2 + 1));
	if (!res)
		return (ft_free(s1));
	if (s1)
		ft_strlcpy(res, s1, len1 + 1);
	else
		res[0] = '\0';
	ft_strlcpy(res + len1, s2, len2 + 1);
	free(s1);
	return (res);
}

char	*ft_free(char *s)
{
	free(s);
	return (NULL);
}
