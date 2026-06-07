#include "rush.h"

void	handle_below_20(char *str, int size, char *dict, int *first)
{
	char	tmp[3];
	int		tens;

	tens = 0;
	if (size >= 2)
		tens = (str[size - 2] - '0') * 10 + (str[size - 1] - '0');
	else
		tens = str[0] - '0';
	if (tens >= 10)
	{
		tmp[0] = str[size - 2];
		tmp[1] = str[size - 1];
		tmp[2] = '\0';
	}
	else
	{
		tmp[0] = str[size - 1];
		tmp[1] = '\0';
	}
	print_from_dict(dict, tmp, first);
}

void	handle_20_plus(char *str, int size, char *dict, int *first)
{
	char	tmp[3];

	tmp[0] = str[size - 2];
	tmp[1] = '0';
	tmp[2] = '\0';
	print_from_dict(dict, tmp, first);
	if (str[size - 1] != '0')
	{
		tmp[0] = str[size - 1];
		tmp[1] = '\0';
		print_from_dict(dict, tmp, first);
	}
}

void	handle_3_digits(char *str, int size, char *dict, int *first)
{
	char	tmp[3];
	int		tens;

	if (size == 3 && str[0] != '0')
	{
		tmp[0] = str[0];
		tmp[1] = '\0';
		print_from_dict(dict, tmp, first);
		print_from_dict(dict, "100", first);
	}
	if (size >= 2)
	{
		tens = (str[size - 2] - '0') * 10;
		tens = tens + (str[size - 1] - '0');
	}
	else
		tens = str[0] - '0';
	if (tens == 0)
		return ;
	if (tens <= 19)
		handle_below_20(str, size, dict, first);
	else
		handle_20_plus(str, size, dict, first);
}
