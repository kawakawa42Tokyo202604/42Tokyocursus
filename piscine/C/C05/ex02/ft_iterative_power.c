int	ft_iterative_power(int nb, int power)
{
	int	answer;

	answer = 1;
	if (power < 0)
		return (0);
	while (power > 0)
	{
		answer = answer * nb;
		--power;
	}
	return (answer);
}

// #include <stdio.h>

// int	main(void)
// {
// 	printf("%d", ft_iterative_power(5, 6));
// }
