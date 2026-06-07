#include "push_swap.h"

static size_t	ft_base_len(char *base)
{
	size_t	i;

	i = 0;
	while (base[i])
		i++;
	return (i);
}

int	ft_putnbr_base(unsigned long n, char *base, int fd)
{
	int				count;
	unsigned long	base_len;

	count = 0;
	base_len = ft_base_len(base);
	if (n >= base_len)
		count += ft_putnbr_base(n / base_len, base, fd);
	count += write (fd, &base[n % base_len], 1);
	return (count);
}
