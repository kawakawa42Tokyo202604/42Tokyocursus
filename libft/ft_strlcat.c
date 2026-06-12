#include "libft.h"

size_t	ft_strlcat(char *dst, const char *src, size_t size)
{
	size_t	d_len;
	size_t	s_len;
	size_t	i;

	d_len = 0;
	while (dst[d_len] != '\0' && d_len < size)
		++d_len;
	s_len = 0;
	while (src[s_len] != '\0')
		++s_len;
	if (size <= d_len)
	{
		return (size + s_len);
	}
	i = 0;
	while (src[i] != '\0' && (d_len + i + 1) < size)
	{
		dst[d_len + i] = src[i];
		++i;
	}
	dst[d_len + i] = '\0';
	return (d_len + s_len);
}
