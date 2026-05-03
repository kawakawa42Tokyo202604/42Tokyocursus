#include <stdlib.h>

char	**ft_split(char *str, char *charset)
{
	if (ft_strlen(str) <= 0 && ft_strlen(charset) <= 0)
		return ;
}

int	ft_strlen(char *str)
{
	int	i;

	i = 0;
	while (str[i] != '\0')
		++i;
	return (i);
}

int	ft_sepcount(char *str, char *sep)
{
	int	i;
	int	j;
	int	count;

	i = 0;
	while (str[i] != '\0')
	{
		if (str[i] == sep[0])
		{
			j = 0;
			while (str[i + j]!='\0' && str[i + j] == sep[j])
				++j;
			if (sep[j] == '\0')
				++count;
		}
		++i;
	}
}
