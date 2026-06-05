#include "push_swap.h"

void	init_stack(t_stack **stack_a, int *array, int *tmp, int size)
{
	int	i;

	i = -1;
	sort_array(tmp, size);
	while (++i < size)
		ft_lstadd_back(stack_a, ft_lstnew(array[i],
				find_index(array[i], tmp, size)));
}

int	find_index(int val, int *sorted_array, int size)
{
	int	i;

	i = 0;
	while (i < size)
	{
		if (val == sorted_array[i])
			return (i);
		i++;
	}
	return (-1);
}

void	sort_array(int *tmp, int size)
{
	int	i;
	int	j;
	int	s;

	i = 0;
	while (i < size - 1)
	{
		j = i + 1;
		while (j < size)
		{
			if (tmp[i] > tmp[j])
			{
				s = tmp[i];
				tmp[i] = tmp[j];
				tmp[j] = s;
			}
			j++;
		}
		i++;
	}
}
