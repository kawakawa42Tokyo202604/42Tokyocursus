#include "rush.h"

void	get_chunk(char *dest, char *src, int size)
{
	int	i;

	i = 0;
	while (i < size)
	{
		dest[i] = src[i];
		i++;
	}
	dest[i] = '\0';
}

int	solve(char *str, char *dict)
{
	int		len;
	int		first;

	first = 1;
	len = ft_strlen(str);
	if (!check_all_units(dict, len))
		return (write(2, "Dict Error\n", 11) * 0);
	if (len == 1 && str[0] == '0')
	{
		if (print_from_dict(dict, "0", &first) == 0)
			return (1);
		return (write(2, "Dict Error\n", 11) * 0);
	}
	while (len > 0)
		process_chunk(&str, dict, &len, &first);
	return (1);
}

int	validate_dict(char *path)
{
	int	fd;

	fd = open(path, O_RDONLY);
	if (fd < 0)
		return (0);
	close(fd);
	return (1);
}

int	is_numeric(char *str)
{
	int	i;

	i = 0;
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (0);
		i++;
	}
	return (1);
}

int	main(int argc, char **argv)
{
	char	*dict;
	char	*num;

	dict = "numbers.dict";
	if (argc == 2)
		num = argv[1];
	else if (argc == 3 && !is_numeric(argv[1]))
	{
		dict = argv[1];
		num = argv[2];
	}
	else
		return (write(2, "Error\n", 6) && 1);
	if (validate_dict(dict) == 0)
		return (write(2, "Error\n", 6) && 1);
	if (check_and_trim(&num))
		return (1);
	if (solve(num, dict))
		write(1, "\n", 1);
	return (0);
}
