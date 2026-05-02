/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strcpy.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawaka <takawaka@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/25 10:49:37 by takawaka          #+#    #+#             */
/*   Updated: 2026/03/25 13:56:34 by takawaka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//#include <unistd.h>

// void	print_array(char *dest);
// char	*ft_strcpy(char *dest, char *src);

char	*ft_strcpy(char *dest, char *src)
{
	char	*begin;

	begin = dest;
	while (*src)
	{
		*dest = *src;
		++dest;
		++src;
	}
	*dest = 0;
	return (begin);
}

// int	main(void)
// {
// 	char	dest[50];
// 	char	*src;

// 	ft_strcpy(dest, "banana");
// 	src = "apple";
// 	print_array(dest);
// 	write(1, "\n", 1);
// 	ft_strcpy(dest, src);
// 	print_array(dest);
// 	write(1, "\n", 1);
// 	return (0);
// }

// void	print_array(char *dest)
// {
// 	while (*dest)
// 	{
// 		write(1, dest, 1);
// 		dest++;
// 	}
// }
