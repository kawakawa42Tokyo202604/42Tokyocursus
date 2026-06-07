#include "push_swap.h"

static int	get_max_bits(int size)
{
	int	max_bits;
	int	max_num;

	max_bits = 0;
	max_num = size - 1;
	while ((max_num >> max_bits) != 0)
		max_bits++;
	return (max_bits);
}

void	sort_complex(t_stack **a, t_stack **b, int size, t_config *config)
{
	int	i;
	int	j;
	int	max_bits;

	i = 0;
	max_bits = get_max_bits(size);
	while (i < max_bits)
	{
		j = 0;
		while (j < size)
		{
			if ((((*a)->index >> i) & 1) == 0)
				pb(a, b, config);
			else
				ra(a, config);
			j++;
		}
		while (*b)
			pa(a, b, config);
		i++;
	}
}
