#include "push_swap.h"

#include "push_swap.h"

long	ft_atoi(const char *nptr)
{
	long	sign;
	long	value;
	size_t	i;

	i = 0;
	while (nptr[i] == ' ' || (nptr[i] >= 9 && nptr[i] <= 13))
		i++;
	sign = 1;
	if (nptr[i] == '+' || nptr[i] == '-')
		if (nptr[i++] == '-')
			sign = -1;
	while (nptr[i] == '0')
		i++;
	if (!(nptr[i] >= '0' && nptr[i] <= '9'))
		return (2147483648L);
	value = 0;
	while (nptr[i] >= '0' && nptr[i] <= '9')
	{
		value = (value * 10) + (nptr[i++] - '0');
		if (value > (long)INT_MAX + (sign == -1))
			return (2147483648L);
	}
	return (value * (nptr[i] == '\0') * sign + 2147483648L * (nptr[i] != '\0'));
}
