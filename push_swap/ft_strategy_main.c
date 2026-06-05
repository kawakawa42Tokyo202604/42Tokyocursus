#include "push_swap.h"

static void	print_stats(t_config *config)
{
	ft_dprintf(2, "[bench] sa: %d sb: %d ss: %d\n",
		config->stats.sa, config->stats.sb, config->stats.ss);
	ft_dprintf(2, "[bench] pa: %d pb: %d\n",
		config->stats.pa, config->stats.pb);
	ft_dprintf(2, "[bench] ra: %d rb: %d rr: %d\n",
		config->stats.ra, config->stats.rb, config->stats.rr);
	ft_dprintf(2, "[bench] rra: %d rrb: %d rrr: %d\n",
		config->stats.rra, config->stats.rrb, config->stats.rrr);
}

static void	print_bench(t_config *config, double disorder)
{
	char	*strat;
	char	*comp;
	int		pct;

	strat = "Adaptive";
	comp = "Mixed";
	if (config->strategy == SIMPLE)
		(1 && (strat = "Simple") && (comp = "O(n^2)"));
	else if (config->strategy == MEDIUM)
		(1 && (strat = "Medium") && (comp = "O(n√n)"));
	else if (config->strategy == COMPLEX)
		(1 && (strat = "Complex") && (comp = "O(n log n)"));
	pct = (int)(disorder * 10000.0 + 0.5);
	ft_dprintf(2, "[bench] disorder: %d.%d%d%%\n",
		pct / 100, (pct / 10) % 10, pct % 10);
	ft_dprintf(2, "[bench] strategy: %s / %s\n", strat, comp);
	ft_dprintf(2, "[bench] total_ops: %d\n", config->stats.total);
	print_stats(config);
}

void	excute_strategy(t_stack **stack_a, t_config *config, int size)
{
	t_stack		*stack_b;
	double		disorder;

	if (size < 2 || is_sorted(*stack_a))
		return ;
	stack_b = NULL;
	disorder = compute_disorder(*stack_a, size);
	if (size <= 5 || config->strategy == SIMPLE
		|| (config->strategy == ADAPTIVE && disorder < 0.2))
		sort_simple(stack_a, &stack_b, size, config);
	else if (config->strategy == MEDIUM
		|| (config->strategy == ADAPTIVE && (disorder < 0.8 || size <= 500)))
		sort_medium(stack_a, &stack_b, size, config);
	else
		sort_complex(stack_a, &stack_b, size, config);
	if (config->bench)
		print_bench(config, disorder);
}
