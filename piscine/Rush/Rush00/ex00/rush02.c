#include <unistd.h>

//y=row x=column
void	if_corner(int row, int col, int x, int y);
void	if_center(int row, int col, int x, int y);
void	ft_putchar(char c);

void	rush(int x, int y)
{
	int	row;
	int	col;

	if (x <= 0 || y <= 0)
		return ;
	else
	{
		row = 1;
		while (row <= y)
		{
			col = 1;
			while (col <= x)
			{
				if_corner(row, col, x, y);
				if_center(row, col, x, y);
				if (col == 2147483647)
					break ;
				++col;
			}
			ft_putchar('\n');
			if (row == 2147483647)
				break ;
			++row;
		}
	}
}

void	if_corner(int row, int col, int x, int y)
{
	if (row == 1 && col == 1)
	{
		ft_putchar('A');
	}
	else if (row == 1 && col == x)
	{
		ft_putchar('A');
	}
	else if (row == y && col == 1)
	{
		ft_putchar('C');
	}
	else if (row == y && col == x)
	{
		ft_putchar('C');
	}
}

void	if_center(int row, int col, int x, int y)
{
	if (row == 1 && (col > 1 && col < x))
	{
		ft_putchar('B');
	}
	else if ((row > 1 && row < y) && (col > 1 && col < x))
	{
		ft_putchar(' ');
	}
	else if (row == y && (col > 1 && col < x))
	{
		ft_putchar('B');
	}
	else if (col == 1 && (row < y && row > 1))
	{
		ft_putchar('B');
	}
	else if (col == x && (row < y && row > 1))
	{
		ft_putchar('B');
	}
}
