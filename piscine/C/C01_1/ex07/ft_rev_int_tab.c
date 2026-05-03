//#include <unistd.h>

void	ft_rev_int_tab(int *tab, int size)
{
	int	temp;
	int	i;

	i = 0;
	while (i < (size / 2))
	{
		temp = tab[i];
		tab[i] = tab[size - 1 - i];
		tab[size - 1 - i] = temp;
		i++;
	}
}

// int	main(void)
// {
// 	int		tab[] = {1, 2, 3, 4, 5, 6};
// 	int		size;
// 	int		i;
// 	char	c;

// 	size = 6;
// 	i = 0;
// 	ft_rev_int_tab(tab, size);
// 	while (i < size)
// 	{
// 		c = tab[i] + '0';
// 		write(1, &c, 1);
// 		i++;
// 	}
// 	write(1, "\n", 1);
// }
