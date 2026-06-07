#include <stdlib.h>

int	get_res_len(long n, int base_len)
{
	int	len;

	len = 0;
	if (n <= 0)
	{
		len++;
		if (n < 0)
			n = -n;
	}
	while (n > 0)
	{
		n = n / base_len;
		len++;
	}
	return (len);
}

char	*ft_int_to_base(int int_base, char *base_to, int base_len)
{
	long	n;
	int		len;
	char	*res;

	n = int_base;
	len = get_res_len(n, base_len);
	res = (char *)malloc(sizeof(char) * (len + 1));
	if (!res)
		return (NULL);
	res[len--] = '\0';
	if (n == 0)
		res[0] = base_to[0];
	if (n < 0)
	{
		res[0] = '-';
		n = -n;
	}
	while (n > 0)
	{
		res[--len] = base_to[n % base_len];
		n = n / base_len;
	}
	return (res);
}
