int	ft_sqrt(int nb)
{
	int	n;

	n = 1;
	while (n <= 46340 && n * n <= nb)
	{
		if (n * n == nb)
			return (n);
		++n;
	}
	return (0);
}

// #include <stdio.h>

// int	main(void)
// {
// 	printf("%d", ft_sqrt(64));
// }
