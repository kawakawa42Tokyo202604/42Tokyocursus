/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strupcase.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawaka <takawaka@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/25 17:31:15 by takawaka          #+#    #+#             */
/*   Updated: 2026/03/25 18:20:08 by takawaka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// #include <unistd.h>

char	*ft_strupcase(char *str)
{
	char	*start;

	start = str;
	while (*str != '\0')
	{
		if ('a' <= *str && *str <= 'z')
		{
			*str = *str - 32;
		}
		++str;
	}
	return (start);
}

// int	main(void)
// {
// 	char	str[30] = "toumaHHdgsyuA";

// 	*ft_strupcase(str);
// }
