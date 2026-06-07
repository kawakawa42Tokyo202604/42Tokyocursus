#include "push_swap.h"

static void	clear_gnl(void)
{
	char	*line;

	while (1)
	{
		line = get_next_line(0);
		if (!line)
			break ;
		free(line);
	}
}

static int	execute_rotate(t_stack **a, t_stack **b, char *line)
{
	if (ft_strcmp(line, "ra\n") == 0)
		ft_rotate(a);
	else if (ft_strcmp(line, "rb\n") == 0)
		ft_rotate(b);
	else if (ft_strcmp(line, "rr\n") == 0)
	{
		ft_rotate(a);
		ft_rotate(b);
	}
	else if (ft_strcmp(line, "rra\n") == 0)
		ft_reverse_rotate(a);
	else if (ft_strcmp(line, "rrb\n") == 0)
		ft_reverse_rotate(b);
	else if (ft_strcmp(line, "rrr\n") == 0)
	{
		ft_reverse_rotate(a);
		ft_reverse_rotate(b);
	}
	else
		return (0);
	return (1);
}

static int	execute_line(t_stack **a, t_stack **b, char *line)
{
	if (ft_strcmp(line, "sa\n") == 0)
		ft_swap(a);
	else if (ft_strcmp(line, "sb\n") == 0)
		ft_swap(b);
	else if (ft_strcmp(line, "ss\n") == 0)
	{
		ft_swap(a);
		ft_swap(b);
	}
	else if (ft_strcmp(line, "pa\n") == 0)
		ft_push(a, b);
	else if (ft_strcmp(line, "pb\n") == 0)
		ft_push(b, a);
	else
		return (execute_rotate(a, b, line));
	return (1);
}

static void	run_checker(t_stack **a, t_stack **b, t_config *config)
{
	char	*line;

	while (1)
	{
		line = get_next_line(0);
		if (!line)
			break ;
		if (!execute_line(a, b, line))
		{
			free(line);
			clear_gnl();
			write(2, "Error\n", 6);
			free_stack(a);
			free_stack(b);
			free(config->array);
			free(config->tmp);
			exit(1);
		}
		free(line);
	}
	if (is_sorted(*a) && !*b)
		write(1, "OK\n", 3);
	else
		write(1, "KO\n", 3);
}

int	main(int argc, char **argv)
{
	t_config	config;
	t_stack		*stack_a;
	t_stack		*stack_b;
	int			size;

	if (argc < 2)
		return (0);
	stack_a = NULL;
	stack_b = NULL;
	parse_flags(argc, argv, &config);
	size = count_total_elements(argc, argv, config.start_idx);
	if (size < 0)
	{
		write(2, "Error\n", 6);
		return (1);
	}
	if (size == 0)
		return (0);
	if (!parse_and_fill_array(argc, argv, &config, size))
		return (1);
	init_stack(&stack_a, config.array, config.tmp, size);
	run_checker(&stack_a, &stack_b, &config);
	free_stack(&stack_a);
	free_stack(&stack_b);
	return (0);
}
