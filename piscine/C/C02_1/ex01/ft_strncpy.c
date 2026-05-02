/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strncpy.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawaka <takawaka@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/25 12:31:54 by takawaka          #+#    #+#             */
/*   Updated: 2026/03/26 09:00:15 by takawaka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//#include <unistd.h>

// void	print_array(char *dest, unsigned int n);
// char *ft_strncpy(char *dest, char *src, unsigned int n);

char	*ft_strncpy(char *dest, char *src, unsigned int n)
{
	char			*begin;
	unsigned int	i;

	begin = dest;
	i = 0;
	while (*src != 0 && i < n)
	{
		*dest = *src;
		++dest;
		++src;
		++i;
	}
	while (i < n)
	{
		*dest = 0;
		++dest;
		++i;
	}
	return (begin);
}

// int	main(void)
// {
// 	char	dest[50] = "banana";
// 	char	src[50] = "apple";
// unsigned int n;
// n = 10;
// 	print_array(dest, n);
// 	write(1, "\n", 1);
// 	ft_strncpy(dest, src, n);
// 	print_array(dest, n);
// 	write(1, "\n", 1);
// 	return (0);
// }

// void	print_array(char *dest, unsigned  int n)
// {
// unsigned int i;

//i = 0;
// 	while (i < n)
// 	{
// 		write(1, &dest[i], 1);
//    	++i;
// 	}
// }
