/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_is_negative.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawaka <takawaka@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 10:03:18 by takawaka          #+#    #+#             */
/*   Updated: 2026/03/23 11:26:09 by takawaka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>

void	ft_is_negative(int n)
{
	char	ne;
	char	p;

	ne = 'N';
	p = 'P';
	if (n < 0)
	{
		write(1, &ne, 1);
	}
	else
	{
		write(1, &p, 1);
	}
}

// int	main(void)
// {
// 	ft_is_negative(8);
// }
