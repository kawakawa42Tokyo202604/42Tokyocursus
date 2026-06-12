#include "libft.h"

static int	count_words(char const *s, char c)
{
	int	count;

	count = 0;
	while (*s)
	{
		while (*s && *s == c)
			++s;
		if (*s)
		{
			count++;
			while (*s && *s != c)
				++s;
		}
	}
	return (count);
}

static size_t	get_word_len(char const *s, char c)
{
	size_t	len;

	len = 0;
	while (s[len] && s[len] != c)
		++len;
	return (len);
}

static char	**free_all(char **res, size_t i)
{
	while (i > 0)
		free(res[--i]);
	free(res);
	return (NULL);
}

char	**ft_split(char const *s, char c)
{
	char	**res;
	size_t	i;
	size_t	len;

	if (!s)
		return (NULL);
	res = (char **)malloc(sizeof(char *) * (count_words(s, c) + 1));
	if (!res)
		return (NULL);
	i = 0;
	while (*s)
	{
		if (*s != c)
		{
			len = get_word_len(s, c);
			res[i++] = ft_substr(s, 0, len);
			if (!res[i - 1])
				return (free_all(res, i - 1));
			s += len;
		}
		else
			++s;
	}
	res[i] = NULL;
	return (res);
}
