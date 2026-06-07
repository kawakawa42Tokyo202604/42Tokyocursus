#include <unistd.h>

int		check_arg(char *str);
int		count_visible(int *line);
int		check_rows(int grid[4][4], int hints[4][4]);
int		check_cols(int grid[4][4], int hints[4][4]);
int		is_safe(int grid[4][4], int pos, int num);
void	parse_hints(char *str, int hints[4][4]);
int		solve(int grid[4][4], int hints[4][4], int pos);
void	ft_putgrid(int grid[4][4]);
int		main(int argc, char **argv);

int	solve(int grid[4][4], int hints[4][4], int pos)
{
	int	num;

	if (pos == 16)
		return (check_rows(grid, hints) && check_cols(grid, hints));
	num = 0;
	while (++num <= 4)
	{
		if (is_safe(grid, pos, num))
		{
			grid[pos / 4][pos % 4] = num;
			if (solve(grid, hints, pos + 1))
				return (1);
			grid[pos / 4][pos % 4] = 0;
		}
	}
	return (0);
}

int	is_safe(int grid[4][4], int pos, int num)
{
	int	i;

	i = -1;
	while (++i < 4)
	{
		if (grid[pos / 4][i] == num || grid[i][pos % 4] == num)
			return (0);
	}
	return (1);
}
