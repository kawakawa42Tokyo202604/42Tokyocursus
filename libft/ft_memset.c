#include "libft.h"

void	*ft_memset(void *s, int c, size_t n)
{
	size_t			i;
	unsigned char	*p;
	unsigned char	value;

	i = 0;
	p = (unsigned char *)s;
	value = (unsigned char)c;
	while (i < n)
	{
		p[i] = value;
		++i;
	}
	return (s);
}
