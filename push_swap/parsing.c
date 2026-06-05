#include "push_swap.h"

static void	free_and_exit(char **split_args, int j, t_config *config)
{
	while (split_args[j])
	{
		free(split_args[j]);
		j++;
	}
	free(split_args);
	free(config->array);
	free(config->tmp);
	write(2, "Error\n", 6);
	exit(1);
}

static void	set_strategy(char *arg, t_config *config)
{
	if (ft_strcmp(arg, "--simple") == 0)
		config->strategy = SIMPLE;
	else if (ft_strcmp(arg, "--medium") == 0)
		config->strategy = MEDIUM;
	else if (ft_strcmp(arg, "--complex") == 0)
		config->strategy = COMPLEX;
	else if (ft_strcmp(arg, "--bench") == 0)
		config->bench = 1;
}

int	parse_flags(int argc, char **argv, t_config *config)
{
	int	i;

	i = 1;
	config->start_idx = 1;
	config->strategy = ADAPTIVE;
	config->bench = 0;
	ft_bzero(&(config->stats), sizeof(t_stats));
	while (i < argc)
	{
		if (argv[i][0] == '-' && !(argv[i][1] >= '0' && argv[i][1] <= '9'))
		{
			if (!is_flag(argv[i]))
			{
				write(2, "Error\n", 6);
				exit(1);
			}
			set_strategy(argv[i], config);
		}
		i++;
	}
	return (1);
}

int	is_sorted(t_stack *stack)
{
	if (!stack)
		return (1);
	while (stack->next)
	{
		if (stack->index > stack->next->index)
			return (0);
		stack = stack->next;
	}
	return (1);
}

int	fill_one_arg(char *arg, t_config *config, int *k)
{
	char	**split_args;
	long	val;
	int		j;

	if (is_flag(arg))
		return (1);
	split_args = ft_split(arg);
	if (!split_args)
		return (0);
	j = 0;
	while (split_args[j])
	{
		val = ft_atoi(split_args[j]);
		if (val == 2147483648L)
			free_and_exit(split_args, j, config);
		config->array[*k] = (int)val;
		config->tmp[*k] = (int)val;
		free(split_args[j++]);
		(*k)++;
	}
	free(split_args);
	return (1);
}
