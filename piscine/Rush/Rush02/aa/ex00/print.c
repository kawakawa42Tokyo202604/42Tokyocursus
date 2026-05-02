/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawaka <takawaka@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/05 15:49:07 by takawaka          #+#    #+#             */
/*   Updated: 2026/04/05 20:26:54 by taisuzuk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rush.h"

int	print_value(int fd, char c, int *first)
{
	if (*first == 0)
		write(1, " ", 1);
	while (c >= 32 && c <= 126)
	{
		write(1, &c, 1);
		if (read(fd, &c, 1) <= 0 || c == '\n')
			break ;
	}
	*first = 0;
	return (1);
}

int	print_from_dict(char *path, char *target, int *first)
{
	int		fd;
	char	c;
	char	buf[256];
	int		res;

	fd = open(path, O_RDONLY);
	if (fd < 0)
		return (1);
	while (read(fd, &c, 1) > 0)
	{
		if (c == '\n')
			continue ;
		res = read_key(fd, &c, buf);
		if (res == -1)
			break ;
		if (ft_strcmp(buf, target) == 0 && print_value(fd, c, first))
		{
			close(fd);
			return (0);
		}
		while (c != '\n' && read(fd, &c, 1) > 0)
			;
	}
	close(fd);
	return (1);
}

int	is_key_in_dict(char *path, char *target)
{
	int		fd;
	char	c;
	char	buf[256];
	int		found;

	found = 0;
	fd = open(path, O_RDONLY);
	if (fd < 0)
		return (0);
	while (read(fd, &c, 1) > 0)
	{
		c = read_key(fd, &c, buf);
		if (ft_strcmp(buf, target) == 0)
		{
			found = 1;
			break ;
		}
		while (c != '\n' && read(fd, &c, 1) > 0)
			;
	}
	close(fd);
	return (found);
}

void	print_unit(int zero_count, char *dict, int *first)
{
	char	unit[256];
	int		i;

	i = 0;
	unit[i++] = '1';
	while (zero_count-- > 0)
		unit[i++] = '0';
	unit[i] = '\0';
	print_from_dict(dict, unit, first);
}
