#include "push_swap.h"

void	ft_rotate(t_stack **stack)
{
	t_stack	*first;
	t_stack	*last;

	if (!stack || !*stack || !(*stack)->next)
		return ;
	first = *stack;
	last = ft_lstlast(*stack);
	*stack = first->next;
	(*stack)->prev = NULL;
	last->next = first;
	first->prev = last;
	first->next = NULL;
}

void	ra(t_stack **stack_a, t_config *config)
{
	ft_rotate(stack_a);
	write(1, "ra\n", 3);
	if (config && config->bench)
	{
		config->stats.ra++;
		config->stats.total++;
	}
}

void	rb(t_stack **stack_b, t_config *config)
{
	ft_rotate(stack_b);
	write(1, "rb\n", 3);
	if (config && config->bench)
	{
		config->stats.rb++;
		config->stats.total++;
	}
}

void	rr(t_stack **stack_a, t_stack **stack_b, t_config *config)
{
	ft_rotate(stack_a);
	ft_rotate(stack_b);
	write(1, "rr\n", 3);
	if (config && config->bench)
	{
		config->stats.rr++;
		config->stats.total++;
	}
}
