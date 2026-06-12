#include "libft.h"

int	ft_atoi(const char *nptr)
{
	int					i;
	unsigned long long	value;
	int					sign;

	i = 0;
	sign = 1;
	value = 0;
	while (nptr[i] == ' ' || (nptr[i] >= 9 && nptr[i] <= 13))
		++i;
	if (nptr[i] == '-' || nptr[i] == '+')
	{
		if (nptr[i] == '-')
			sign = -1;
		++i;
	}
	while (nptr[i] >= '0' && nptr[i] <= '9')
	{
		value = (value * 10) + (nptr[i] - '0');
		if (value > 9223372036854775807ULL && sign == 1)
			return (-1);
		if (value > 9223372036854775808ULL && sign == -1)
			return (0);
		++i;
	}
	return (value * sign);
}
