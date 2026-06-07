#include "push_swap.h"

double	compute_disorder(t_stack *stack, int size)
{
	int		mistakes;
	int		total_pairs;
	t_stack	*i;
	t_stack	*j;

	if (size <= 1)
		return (0.0);
	mistakes = 0;
	total_pairs = 0;
	i = stack;
	while (i)
	{
		j = i->next;
		while (j)
		{
			total_pairs++;
			if (i->index > j->index)
				mistakes++;
			j = j->next;
		}
		i = i->next;
	}
	return ((double)mistakes / total_pairs);
}
