/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   check.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawaka <takawaka@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/29 09:53:51 by takawaka          #+#    #+#             */
/*   Updated: 2026/03/29 12:56:40 by takawaka         ###   ########.fr       */
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

int	count_visible(int *line)
{
	int	i;
	int	max;
	int	count;

	i = -1;
	max = 0;
	count = 0;
	while (++i < 4)
	{
		if (line[i] > max)
		{
			max = line[i];
			++count;
		}
	}
	return (count);
}

int	check_rows(int grid[4][4], int hints[4][4])
{
	int	i;
	int	j;
	int	line[4];

	i = -1;
	while (++i < 4)
	{
		if (count_visible(grid[i]) != hints[2][i])
			return (0);
		j = -1;
		while (++j < 4)
			line[j] = grid[i][3 - j];
		if (count_visible(line) != hints[3][i])
			return (0);
	}
	return (1);
}

int	check_cols(int grid[4][4], int hints[4][4])
{
	int	i;
	int	j;
	int	line[4];

	i = -1;
	while (++i < 4)
	{
		j = -1;
		while (++j < 4)
			line[j] = grid[j][i];
		if (count_visible(line) != hints[0][i])
			return (0);
		j = -1;
		while (++j < 4)
			line[j] = grid[3 - j][i];
		if (count_visible(line) != hints[1][i])
			return (0);
	}
	return (1);
}
