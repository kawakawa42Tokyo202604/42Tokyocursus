#include "libft.h"

char	*ft_strnstr(const char *big, const char *little, size_t len)
{
	size_t	i;
	size_t	j;

	if (!*little)
		return ((char *)big);
	i = 0;
	while (big[i] != '\0' && i < len)
	{
		j = 0;
		if (big[i] == little[j])
		{
			while (little[j] && (i + j) < len && big[i + j] == little[j])
				++j;
			if (little[j] == '\0')
			{
				return ((char *)&big[i]);
			}
		}
		++i;
	}
	return (NULL);
}
