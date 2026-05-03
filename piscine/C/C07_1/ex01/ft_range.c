#include <stdlib.h>

int	*ft_range(int min, int max)
{
	int	*array;
	int	i;
	int	j;

	if (min >= max)
		return (NULL);
	j = 0;
	array = (int *)malloc(sizeof(int) * (max - min));
	if (array == 0)
		return (NULL);
	i = min - 1;
	while (++i < max)
	{
		array[j] = i;
		++j;
	}
	return (array);
}

// #include <stdio.h>

// int	main(void)
// {
// 	int	min;
// 	int	max;
// 	int	*array;
// 	int	i;

// 	min = 10;
// 	max = 20;
// 	array = ft_range(min, max);
// 	i = 0;
// 	while (i < max - min)
// 	{
// 		printf("%d", array[i]);
// 		printf("\n");
// 		++i;
// 	}
// }
