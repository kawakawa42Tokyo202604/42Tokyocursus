#include "push_swap.h"

void	sort_three(t_stack **a, t_config *config)
{
	int	first;
	int	second;
	int	third;

	first = (*a)->index;
	second = (*a)->next->index;
	third = (*a)->next->next->index;
	if (first > second && second < third && first < third)
		sa(a, config);
	else if (first > second && second > third)
	{
		sa(a, config);
		rra(a, config);
	}
	else if (first > second && second < third && first > third)
		ra(a, config);
	else if (first < second && second > third && first < third)
	{
		sa(a, config);
		ra(a, config);
	}
	else if (first < second && second > third && first > third)
		rra(a, config);
}

static void	bring_index_top(t_stack **a, int target_idx,
							int size, t_config *config)
{
	t_stack	*tmp;
	int		pos;

	pos = 0;
	tmp = *a;
	while (tmp && tmp->index != target_idx && ++pos)
		tmp = tmp->next;
	if (pos <= size / 2)
		while ((*a)->index != target_idx)
			ra(a, config);
	else
		while ((*a)->index != target_idx)
			rra(a, config);
}

void	sort_simple(t_stack **a, t_stack **b, int size, t_config *config)
{
	int	current_size;
	int	target_idx;

	if (size == 2 && (*a)->index > (*a)->next->index)
		sa(a, config);
	else if (size == 3)
		sort_three(a, config);
	else
	{
		current_size = size;
		target_idx = 0;
		while (current_size > 3)
		{
			bring_index_top(a, target_idx, current_size, config);
			pb(a, b, config);
			current_size--;
			target_idx++;
		}
		sort_three(a, config);
		while (*b)
			pa(a, b, config);
	}
}
