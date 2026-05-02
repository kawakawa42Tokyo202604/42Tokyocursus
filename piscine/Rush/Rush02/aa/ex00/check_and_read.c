/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   check_and_read.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawaka <takawaka@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/05 15:47:19 by takawaka          #+#    #+#             */
/*   Updated: 2026/04/05 16:26:25 by takawaka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rush.h"

int	check_and_trim(char **num)
{
	int		i;
	char	*str;

	str = *num;
	i = 0;
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
		{
			write(2, "Error\n", 6);
			return (1);
		}
		i++;
	}
	while (*str == '0' && *(str + 1) != '\0')
		str++;
	*num = str;
	return (0);
}

int	read_key(int fd, char *c, char *buf)
{
	int	i;

	i = 0;
	while (*c >= '0' && *c <= '9' && i < 255)
	{
		buf[i++] = *c;
		if (read(fd, c, 1) <= 0)
			break ;
	}
	buf[i] = '\0';
	while (*c == ' ')
		if (read(fd, c, 1) <= 0)
			break ;
	if (*c != ':')
		return (-1);
	if (read(fd, c, 1) <= 0)
		return (-1);
	while (*c == ' ')
		if (read(fd, c, 1) <= 0)
			break ;
	return (0);
}

void	process_chunk(char **str, char *dict, int *len, int *first)
{
	char	chunk[4];
	int		head;

	head = *len % 3;
	if (head == 0)
		head = 3;
	get_chunk(chunk, *str, head);
	if (!(chunk[0] == '0' && chunk[1] == '0' && chunk[2] == '0'))
	{
		handle_3_digits(chunk, head, dict, first);
		if (*len > 3)
			print_unit(*len - head, dict, first);
	}
	*str += head;
	*len -= head;
}

int	check_all_units(char *dict, int len)
{
	char	unit[256];
	int		i;
	int		z;

	while (len > 3)
	{
		if (len % 3 == 0)
			z = len - 3;
		else
			z = len - (len % 3);
		unit[0] = '1';
		i = 1;
		while (i <= z)
			unit[i++] = '0';
		unit[i] = '\0';
		if (!is_key_in_dict(dict, unit))
			return (0);
		if (len % 3 == 0)
			len -= 3;
		else
			len -= (len % 3);
	}
	return (1);
}
