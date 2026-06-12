#include "libft.h"

char	*ft_strtrim(char const *s1, char const *set)
{
	size_t	i;
	size_t	j;
	size_t	s1_len;

	if (!s1)
		return (NULL);
	s1_len = ft_strlen(s1);
	if (!set)
		return (ft_strdup(s1));
	i = 0;
	j = s1_len;
	while (s1[i] && ft_strchr(set, s1[i]))
		++i;
	if (s1[i] == '\0')
		return (ft_strdup(""));
	j = s1_len - 1;
	while (j > i && ft_strchr(set, s1[j]))
		--j;
	return (ft_substr(s1, i, (size_t)(j - i + 1)));
}
