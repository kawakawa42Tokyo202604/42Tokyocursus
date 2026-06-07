#include "push_swap.h"

void	ft_swap(t_stack **stack)
{
	t_stack	*first;
	t_stack	*second;

	if (!stack || !*stack || !(*stack)->next)
		return ;
	first = *stack;
	second = first->next;
	first->next = second->next;
	if (second->next)
		second->next->prev = first;
	second->next = first;
	second->prev = first->prev;
	first->prev = second;
	*stack = second;
}

void	sa(t_stack **stack_a, t_config *config)
{
	ft_swap(stack_a);
	write(1, "sa\n", 3);
	if (config && config->bench)
	{
		config->stats.sa++;
		config->stats.total++;
	}
}

void	sb(t_stack **stack_b, t_config *config)
{
	ft_swap(stack_b);
	write(1, "sb\n", 3);
	if (config && config->bench)
	{
		config->stats.sb++;
		config->stats.total++;
	}
}

void	ss(t_stack **stack_a, t_stack **stack_b, t_config *config)
{
	ft_swap(stack_a);
	ft_swap(stack_b);
	write(1, "ss\n", 3);
	if (config && config->bench)
	{
		config->stats.ss++;
		config->stats.total++;
	}
}
