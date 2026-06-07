#include "push_swap.h"

#include "push_swap.h"

static int	is_space(char c)
{
	return (c == ' ' || (c >= 9 && c <= 13));
}

int	count_words(char const *s)
{
	int	count;

	count = 0;
	while (*s)
	{
		while (*s && is_space(*s))
			s++;
		if (*s)
		{
			count++;
			while (*s && !is_space(*s))
				s++;
		}
	}
	return (count);
}

static size_t	get_word_len(char const *s)
{
	size_t	len;

	len = 0;
	while (s[len] && !is_space(s[len]))
		len++;
	return (len);
}

static char	**free_all(char **res, size_t i)
{
	while (i > 0)
	{
		free(res[i - 1]);
		i--;
	}
	free(res);
	return (NULL);
}

char	**ft_split(char const *s)
{
	char	**res;
	size_t	i;
	size_t	len;

	if (!s)
		return (NULL);
	res = (char **)malloc(sizeof(char *) * (count_words(s) + 1));
	if (!res)
		return (NULL);
	i = 0;
	while (*s)
	{
		if (is_space(*s))
			s++;
		else
		{
			len = get_word_len(s);
			res[i++] = ft_substr(s, 0, len);
			if (!res[i - 1])
				return (free_all(res, i - 1));
			s += len;
		}
	}
	res[i] = NULL;
	return (res);
}
