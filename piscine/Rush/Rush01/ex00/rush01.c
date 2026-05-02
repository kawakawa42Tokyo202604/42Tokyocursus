/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rush01.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawaka <takawaka@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/28 10:33:46 by takawaka          #+#    #+#             */
/*   Updated: 2026/03/29 16:34:49 by takawaka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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

// allgrid=0
// checkargv
// putgrit number
// checkvisible
// OK confirm
// incorrect restart previous number

void	ft_putgrid(int grid[4][4])
{
	int		i;
	int		j;
	char	c;

	i = -1;
	while (++i < 4)
	{
		j = -1;
		while (++j < 4)
		{
			c = grid[i][j] + '0';
			write(1, &c, 1);
			if (j < 3)
				write(1, " ", 1);
		}
		write(1, "\n", 1);
	}
}

int	check_arg(char *str)
{
	int	i;
	int	count;

	i = 0;
	count = 0;
	while (str[i])
	{
		if ('1' <= str[i] && str[i] <= '4')
			++count;
		else if (str[i] != ' ')
			return (0);
		++i;
	}
	return (count == 16);
}

void	parse_hints(char *str, int hints[4][4])
{
	int	i;
	int	k;

	i = -1;
	k = 0;
	while (++i < 16)
	{
		while (str[k] == ' ' || (str[k] < '1' || str[k] > '4'))
			++k;
		hints[i / 4][i % 4] = str[k] - '0';
		k++;
	}
}

int	main(int argc, char **argv)
{
	int	grid[4][4];
	int	hints[4][4];
	int	i;

	if (argc != 2 || !check_arg(argv[1]))
	{
		write(1, "Error\n", 6);
		return (0);
	}
	i = -1;
	while (++i < 16)
		grid[i / 4][i % 4] = 0;
	parse_hints(argv[1], hints);
	if (solve(grid, hints, 0))
		ft_putgrid(grid);
	else
		write(1, "Error\n", 6);
	return (0);
}
