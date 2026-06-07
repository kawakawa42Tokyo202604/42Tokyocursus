#include "push_swap.h"

int	main(int argc, char **argv)
{
	t_config	config;
	t_stack		*stack_a;
	int			size;

	if (argc < 2)
		return (0);
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
	stack_a = NULL;
	init_stack(&stack_a, config.array, config.tmp, size);
	excute_strategy(&stack_a, &config, size);
	free(config.array);
	free(config.tmp);
	free_stack(&stack_a);
	return (0);
}
