#include <unistd.h>

void	while_count(char a, char b, char c, char d);
void	ft_putcharcomb2(char a, char b, char c, char d);

void	ft_print_comb2(void)
{
	char	a;
	char	b;
	char	c;
	char	d;

	a = '0';
	b = '0';
	c = '0';
	d = '0';
	while_count(a, b, c, d);
}

void	while_count(char a, char b, char c, char d)
{
	a = '0';
	while (a <= '9')
	{
		b = '0';
		while (b <= '9')
		{
			c = '0';
			while (c <= '9')
			{
				d = '0';
				while (d <= '9')
				{
					ft_putcharcomb2(a, b, c, d);
					++d;
				}
				++c;
			}
			++b;
		}
		++a;
	}
}

void	ft_putcharcomb2(char a, char b, char c, char d)
{
	if ((a < c) || (a == c && b < d))
	{
		write(1, &a, 1);
		write(1, &b, 1);
		write(1, " ", 1);
		write(1, &c, 1);
		write(1, &d, 1);
		if (!(a == '9' && b == '8' && c == '9' && d == '9'))
			write(1, ", ", 2);
	}
}

// int	main(void)
// {
// 	ft_print_comb2();
// }
