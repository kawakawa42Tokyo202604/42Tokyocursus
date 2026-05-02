/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_show_tab.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawaka <takawaka@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/08 15:04:19 by takawaka          #+#    #+#             */
/*   Updated: 2026/04/08 15:50:57 by takawaka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_stock_str.h"
#include <unistd.h>

void	ft_putstr(char *str);
void	ft_putsize(int size);

void	ft_show_tab(struct s_stock_str *par)
{
	int	i;

	i = 0;
	while (par[i].str != 0)
	{
		ft_putstr(par[i].str);
		ft_putsize(par[i].size);
		write(1, "\n", 1);
		ft_putstr(par[i].copy);
		++i;
	}
}

void	ft_putstr(char *str)
{
	int	i;

	i = 0;
	while (str[i] != '\0')
		++i;
	write(1, str, i);
	write(1, "\n", 1);
}

void	ft_putsize(int size)
{
	char	c;

	if (size >= 10)
	{
		ft_putsize(size / 10);
	}
	c = size % 10 + '0';
	write(1, &c, 1);
}
