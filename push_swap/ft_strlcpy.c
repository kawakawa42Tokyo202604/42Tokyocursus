#include "push_swap.h"

size_t	ft_strlcpy(char *dst, const char *src, size_t size)
{
	size_t		i;
	size_t		src_len;

	src_len = ft_strlen(src);
	if (size == 0)
		return (src_len);
	i = 0;
	while (src[i] && i < (size - 1))
	{
		dst[i] = src[i];
		i++;
	}
	dst[i] = '\0';
	return (src_len);
}
