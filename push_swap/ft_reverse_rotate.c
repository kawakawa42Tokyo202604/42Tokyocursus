#include "push_swap.h"

void	ft_reverse_rotate(t_stack **stack)
{
	t_stack	*last;

	if (!stack || !*stack || !(*stack)->next)
		return ;
	last = ft_lstlast(*stack);
	last->prev->next = NULL;
	last->next = *stack;
	last->prev = NULL;
	(*stack)->prev = last;
	*stack = last;
}

void	rra(t_stack **stack_a, t_config *config)
{
	ft_reverse_rotate(stack_a);
	write (1, "rra\n", 4);
	if (config && config->bench)
	{
		config->stats.rra++;
		config->stats.total++;
	}
}

void	rrb(t_stack **stack_b, t_config *config)
{
	ft_reverse_rotate(stack_b);
	write (1, "rrb\n", 4);
	if (config && config->bench)
	{
		config->stats.rrb++;
		config->stats.total++;
	}
}

void	rrr(t_stack **stack_a, t_stack **stack_b, t_config *config)
{
	ft_reverse_rotate(stack_a);
	ft_reverse_rotate(stack_b);
	write (1, "rrr\n", 4);
	if (config && config->bench)
	{
		config->stats.rrr++;
		config->stats.total++;
	}
}
