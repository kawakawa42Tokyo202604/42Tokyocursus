#include "libft.h"

static int	get_len(long nbr)
{
	int	len;

	if (nbr == 0)
		return (1);
	len = 0;
	if (nbr < 0)
	{
		++len;
		nbr = -nbr;
	}
	while (nbr > 0)
	{
		nbr /= 10;
		++len;
	}
	return (len);
}

char	*ft_itoa(int n)
{
	char	*s;
	int		len;
	long	nbr;

	nbr = n;
	len = get_len(nbr);
	s = (char *)malloc(sizeof(char) * (len + 1));
	if (!s)
		return (NULL);
	s[len] = '\0';
	if (nbr < 0)
	{
		s[0] = '-';
		nbr = -nbr;
	}
	if (nbr == 0)
		s[0] = '0';
	while (nbr > 0)
	{
		s[--len] = (nbr % 10) + '0';
		nbr /= 10;
	}
	return (s);
}
