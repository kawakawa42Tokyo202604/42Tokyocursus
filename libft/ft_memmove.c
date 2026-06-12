#include "libft.h"

void	*ft_memmove(void *dest, const void *src, size_t n)
{
	size_t			i;
	unsigned char	*d;
	unsigned char	*s;

	i = n;
	if (dest == src)
		return (dest);
	else if (!dest && !src)
		return (NULL);
	else if (dest < src)
	{
		return (ft_memcpy(dest, src, n));
	}
	else
	{
		d = (unsigned char *)dest;
		s = (unsigned char *)src;
		while (i > 0)
		{
			--i;
			d[i] = s[i];
		}
		return (dest);
	}
}
