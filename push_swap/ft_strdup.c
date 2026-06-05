#include "push_swap.h"

char	*ft_strdup(const char *s)
{
	char	*dst;
	size_t	len;
	size_t	i;

	len = ft_strlen(s);
	dst = (char *)malloc(sizeof(char) * (len + 1));
	if (!dst)
		return (NULL);
	i = 0;
	while (s[i])
	{
		dst[i] = s[i];
		i++;
	}
	dst[i] = '\0';
	return (dst);
}
