/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_iterative_power.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawaka <takawaka@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/29 19:28:48 by takawaka          #+#    #+#             */
/*   Updated: 2026/04/02 11:23:07 by takawaka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

int	ft_iterative_power(int nb, int power)
{
	int	answer;

	answer = 1;
	if (power < 0)
		return (0);
	while (power > 0)
	{
		answer = answer * nb;
		--power;
	}
	return (answer);
}

// #include <stdio.h>

// int	main(void)
// {
// 	printf("%d", ft_iterative_power(5, 6));
// }
