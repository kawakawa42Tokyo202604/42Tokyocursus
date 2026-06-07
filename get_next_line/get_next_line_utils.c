#include "get_next_line.h"

char	*all_free(char *b, char *s)
{
	if (b)
		free(b);
	if (s)
		free(s);
	return (NULL);
}

ssize_t	is_newline(char *buf)
{
	ssize_t	i;

	if (!buf)
		return (-1);
	i = -1;
	while (buf[++i])
	{
		if (buf[i] == '\n')
		{
			return (i);
		}
	}
	return (-1);
}

char	*ft_strdupgnl(char *s)
{
	char	*dest;
	size_t	len;
	size_t	i;

	len = ft_strlengnl(s);
	dest = (char *)malloc(sizeof(char) * (len + 1));
	if (!dest)
		return (NULL);
	i = 0;
	while (s[i])
	{
		dest[i] = s[i];
		++i;
	}
	dest[i] = '\0';
	return (dest);
}

ssize_t	ft_strlengnl(char *s)
{
	ssize_t	i;

	if (!s)
		return (0);
	i = 0;
	while (s[i])
	{
		++i;
	}
	return (i);
}

char	*ft_cleangnl(char *s)
{
	free(s);
	return (NULL);
}
