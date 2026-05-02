/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ultimate_div_mod.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawaka <takawaka@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/23 19:39:33 by takawaka          #+#    #+#             */
/*   Updated: 2026/03/25 09:28:52 by takawaka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//#include <unistd.h>

void	ft_ultimate_div_mod(int *a, int *b)
{
	int	ba;

	ba = *a;
	*a = *a / *b;
	*b = ba % *b;
}

// int	main(void)
// {
// 	int		n;
// 	int		m;
// 	char	res_n;
// 	char	res_m;

// 	n = 13;
// 	m = 5;
// 	ft_ultimate_div_mod(&n, &m);
// 	res_n = n + '0';
// 	res_m = m + '0';
// 	write(1, &n, 1);
// 	write(1, &m, 1);
// 	return (0);
// }
