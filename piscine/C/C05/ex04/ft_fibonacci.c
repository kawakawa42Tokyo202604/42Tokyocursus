/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_fibonacci.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawaka <takawaka@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/30 09:59:59 by takawaka          #+#    #+#             */
/*   Updated: 2026/04/02 11:29:55 by takawaka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

int	ft_fibonacci(int index)
{
	int	answer;

	if (index < 0)
		return (-1);
	if (index == 0)
		return (0);
	if (index == 1)
		return (1);
	answer = ft_fibonacci(index - 1) + ft_fibonacci(index - 2);
	return (answer);
}

// #include <stdio.h>

// int	main(void)
// {
// 	printf("%d", ft_fibonacci(5));
// }
