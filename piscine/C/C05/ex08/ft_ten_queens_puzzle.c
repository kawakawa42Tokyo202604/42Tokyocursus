/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ten_queens_puzzle.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawaka <takawaka@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/30 13:37:46 by takawaka          #+#    #+#             */
/*   Updated: 2026/04/02 09:45:13 by takawaka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>

void	ft_putchar(char c);
void	put_num(int *board);
int		solve(int *board, int col);
int		is_safe(int *board, int col);

int	ft_ten_queens_puzzle(void)
{
	int	board[10];
	int	i;

	i = -1;
	while (++i < 10)
		board[i] = 0;
	return (solve(board, 0));
}

int	is_safe(int *board, int col)
{
	int	i;

	i = -1;
	while (++i < col)
	{
		if (board[i] == board[col] || i + board[i] == col + board[col] || i
			- board[i] == col - board[col])
			return (0);
	}
	return (1);
}

int	solve(int *board, int col)
{
	int	row;
	int	count;

	count = 0;
	if (col == 10)
	{
		put_num(board);
		return (1);
	}
	row = -1;
	while (++row < 10)
	{
		board[col] = row;
		if (is_safe(board, col))
		{
			count += solve(board, col + 1);
		}
	}
	return (count);
}

void	put_num(int *board)
{
	int	i;
	int	c;

	i = -1;
	while (++i < 10)
	{
		c = board[i] + '0';
		ft_putchar(c);
	}
	ft_putchar('\n');
}

void	ft_putchar(char c)
{
	write(1, &c, 1);
}

// #include <stdio.h>

// int	main(void)
// {
// 	printf("%d", ft_ten_queens_puzzle());
// 	return (0);
// }
