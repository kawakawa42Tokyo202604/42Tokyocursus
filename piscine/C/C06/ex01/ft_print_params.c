#include <unistd.h>

int	main(int argc, char *argv[])
{
	int	i;
	int	j;

	if (argc < 1)
		return (0);
	i = 0;
	while (++i < argc)
	{
		j = -1;
		while (argv[i][++j] != '\0')
		{
			write(1, &argv[i][j], 1);
		}
		write(1, "\n", 1);
	}
}
