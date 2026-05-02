/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ultimate_range.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawaka <takawaka@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/31 10:40:42 by takawaka          #+#    #+#             */
/*   Updated: 2026/04/06 19:32:09 by takawaka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
//#include <stdlib.h>

int	ft_ultimate_range(int **range, int min, int max)
{
	int	i;
	int	size;
	int	*array;

	if (min >= max)
	{
		*range = (void *)0;
		return (0);
	}
	size = max - min;
	array = (int *)malloc(sizeof(int) * (size));
	if (array == 0)
	{
		*range = (void *)0;
		return (-1);
	}
	i = 0;
	while (min + i < max)
	{
		array[i] = min + i;
		++i;
	}
	*range = array;
	return (size);
}

// int	main(void)
// {
// 	int	*range;
// 	int	min;
// 	int	max;
// 	int	size;
// 	int	i;

// 	min = 10;
// 	max = 20;
// 	size = ft_ultimate_range(&range, min, max);
// 	if (size == -1)
// 		return (1);
// 	i = 0;
// 	while (i < size)
// 	{
// 		printf("%d\n", range[i]);
// 		i++;
// 	}
// 	free(range);
// 	return (0);
// }
