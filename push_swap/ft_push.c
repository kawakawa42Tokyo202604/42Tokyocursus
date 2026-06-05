#include "push_swap.h"

void	ft_push(t_stack **dst, t_stack **src)
{
	t_stack	*node;

	if (!src || !*src)
		return ;
	node = *src;
	*src = node->next;
	if (*src)
		(*src)->prev = NULL;
	node->next = *dst;
	if (*dst)
		(*dst)->prev = node;
	*dst = node;
	(*dst)->prev = NULL;
}

int	pa(t_stack **stack_a, t_stack **stack_b, t_config *config)
{
	ft_push(stack_a, stack_b);
	write(1, "pa\n", 3);
	if (config && config->bench)
	{
		config->stats.pa++;
		config->stats.total++;
	}
	return (1);
}

int	pb(t_stack **stack_a, t_stack **stack_b, t_config *config)
{
	ft_push(stack_b, stack_a);
	write(1, "pb\n", 3);
	if (config && config->bench)
	{
		config->stats.pb++;
		config->stats.total++;
	}
	return (1);
}
