#include "libft.h"

char	*ft_strrchr(const char *s, int c)
{
	size_t	i;

	i = 0;
	while (s[i])
		++i;
	while (1)
	{
		if (s[i] == (char)c)
			return ((char *)&s[i]);
		if (i == 0)
			break ;
		--i;
	}
	return (NULL);
}
