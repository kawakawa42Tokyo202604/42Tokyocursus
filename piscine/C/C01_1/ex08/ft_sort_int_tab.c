/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_sort_int_tab.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawaka <takawaka@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/24 12:10:00 by takawaka          #+#    #+#             */
/*   Updated: 2026/03/29 17:11:59 by takawaka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//#include <unistd.h>

void	ft_sort_int_tab(int *tab, int size)
{
	int	temp;
	int	i;
	int	changed;
	int	*ab;

	ab = tab;
	changed = 1;
	while (changed)
	{
		changed = 0;
		i = 0;
		tab = ab;
		while (i < size - 1)
		{
			if (*tab > *(tab + 1))
			{
				temp = *tab;
				*tab = *(tab + 1);
				*(tab + 1) = temp;
				changed = 1;
			}
			++i;
			++tab;
		}
	}
}

// int	main(void)
// {
// 	int		tab[] = {5, 7, 2, 9, 1, 0};
// 	int		size;
// 	int		i;
// 	char	c;

// 	size = 6;
// 	i = 0;
// 	while (i != size)
// 	{
// 		c = tab[i] + '0';
// 		write(1, &c, 1);
// 		++i;
// 	}
// 	write(1, "\n", 1);
// 	ft_sort_int_tab(&tab[0], size);
// 	i = 0;
// 	while (i != size)
// 	{
// 		c = tab[i] + '0';
// 		write(1, &c, 1);
// 		++i;
// 	}
// }
