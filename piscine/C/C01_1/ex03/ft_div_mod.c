//#include <unistd.h>

void	ft_div_mod(int a, int b, int *div, int *mod)
{
	*div = a / b;
	*mod = a % b;
}

// int	main(void)
// {
// 	int	a;
// 	int	b;
// 	int	div;
// 	int	mod;

// 	a = 99;
// 	b = 3;
// 	ft_div_mod(a, b, &div, &mod);
// 	write(1, &div, 1);
// 	write(1, &mod, 1);
// }
