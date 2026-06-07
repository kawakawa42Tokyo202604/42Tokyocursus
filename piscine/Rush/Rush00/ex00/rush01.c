#include <unistd.h>

// y=row x=column
void	ft_putchar(char c);
void	print_rectangular(int row, int col, int x, int y);
void	intmax(int row, int col, int x, int y);

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
				print_rectangular(row, col, x, y);
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

void	print_rectangular(int row, int col, int x, int y)
{
	if (row == 1 && col == 1)
	{
		ft_putchar('/');
	}
	else if (row == y && col == 1)
	{
		ft_putchar('\\');
	}
	else if (row == 1 && col == x)
	{
		ft_putchar('\\');
	}
	else if (row == y && col == x)
	{
		ft_putchar('/');
	}
	else if ((col > 1 && col < x) && (row > 1 && row < y))
	{
		ft_putchar(' ');
	}
	else if (col == 1 || row == 1 || row == y || col == x)
	{
		ft_putchar('*');
	}
}
