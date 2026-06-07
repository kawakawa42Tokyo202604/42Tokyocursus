#include "push_swap.h"

int	is_flag(char *arg)
{
	if (!arg)
		return (0);
	if (ft_strcmp(arg, "--simple") == 0 || ft_strcmp(arg, "--medium") == 0)
		return (1);
	if (ft_strcmp(arg, "--complex") == 0 || ft_strcmp(arg, "--adaptive") == 0)
		return (1);
	if (ft_strcmp(arg, "--bench") == 0)
		return (1);
	return (0);
}

int	check_dup(int *array, int size)
{
	int	i;
	int	j;

	i = 0;
	while (i < size)
	{
		j = i + 1;
		while (j < size)
		{
			if (array[i] == array[j])
				return (1);
			j++;
		}
		i++;
	}
	return (0);
}

int	parse_and_fill_array(int argc, char **argv,
								t_config *config, int size)
{
	int	i;
	int	k;

	config->array = malloc(sizeof(int) * size);
	config->tmp = malloc(sizeof(int) * size);
	if (!config->array || !config->tmp)
		return (0);
	i = config->start_idx;
	k = 0;
	while (i < argc)
	{
		if (!fill_one_arg(argv[i++], config, &k))
			return (0);
	}
	if (check_dup(config->array, size))
	{
		free(config->array);
		free(config->tmp);
		write(2, "Error\n", 6);
		return (0);
	}
	return (1);
}

int	count_total_elements(int argc, char **argv, int start_idx)
{
	int	total;
	int	i;
	int	words;

	total = 0;
	i = start_idx;
	while (i < argc)
	{
		if (is_flag(argv[i]))
		{
			i++;
			continue ;
		}
		words = count_words(argv[i]);
		if (words == 0)
			return (-1);
		total += words;
		i++;
	}
	return (total);
}
