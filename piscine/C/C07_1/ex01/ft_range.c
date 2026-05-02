/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_range.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawaka <takawaka@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/31 09:56:23 by takawaka          #+#    #+#             */
/*   Updated: 2026/04/05 08:55:27 by takawaka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>

int	*ft_range(int min, int max)
{
	int	*array;
	int	i;
	int	j;

	if (min >= max)
		return (NULL);
	j = 0;
	array = (int *)malloc(sizeof(int) * (max - min));
	if (array == 0)
		return (NULL);
	i = min - 1;
	while (++i < max)
	{
		array[j] = i;
		++j;
	}
	return (array);
}

// #include <stdio.h>

// int	main(void)
// {
// 	int	min;
// 	int	max;
// 	int	*array;
// 	int	i;

// 	min = 10;
// 	max = 20;
// 	array = ft_range(min, max);
// 	i = 0;
// 	while (i < max - min)
// 	{
// 		printf("%d", array[i]);
// 		printf("\n");
// 		++i;
// 	}
// }
