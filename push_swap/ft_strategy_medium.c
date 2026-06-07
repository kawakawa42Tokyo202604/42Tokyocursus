#include "push_swap.h"

static int	get_chunk_size(int size)
{
	if (size <= 50)
		return (8);
	if (size <= 100)
		return (12);
	if (size <= 500)
		return (30);
	return (size / 12);
}

static void	push_to_b(t_stack **a, t_stack **b, int size, t_config *config)
{
	int	i;
	int	chunk;

	i = 0;
	chunk = get_chunk_size(size);
	while (*a)
	{
		if ((*a)->index <= i)
		{
			pb(a, b, config);
			rb(b, config);
			i++;
		}
		else if ((*a)->index <= i + chunk)
		{
			pb(a, b, config);
			i++;
		}
		else
			ra(a, config);
	}
}

static void	pull_to_a(t_stack **a, t_stack **b, t_config *config)
{
	int		size;
	int		pos;
	t_stack	*tmp;

	while (*b)
	{
		size = 0;
		tmp = *b;
		while (tmp && ++size)
			tmp = tmp->next;
		pos = 0;
		tmp = *b;
		while (tmp->index != size - 1)
		{
			pos++;
			tmp = tmp->next;
		}
		if (pos <= size / 2)
			while ((*b)->index != size - 1)
				rb(b, config);
		else
			while ((*b)->index != size - 1)
				rrb(b, config);
		pa(a, b, config);
	}
}

void	sort_medium(t_stack **a, t_stack **b, int size, t_config *config)
{
	push_to_b(a, b, size, config);
	pull_to_a(a, b, config);
}
