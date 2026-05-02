/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlowcase.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawaka <takawaka@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/25 18:18:04 by takawaka          #+#    #+#             */
/*   Updated: 2026/03/25 18:25:30 by takawaka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//#include <unistd.h>

char	*ft_strlowcase(char *str)
{
	char	*start;

	start = str;
	while (*str != '\0')
	{
		if ('A' <= *str && *str <= 'Z')
		{
			*str = *str + 32;
		}
		++str;
	}
	return (start);
}

// int	main(void)
// {
// 	char	str[] = "toumaHHdgsyuA";
// 	char	*res;
// 	int		i;

// 	res = ft_str_is_lowcase(str);
// 	i = 0;
// 	while (res[i] != '\0')
// 	{
// 		write(1, &res[i], 1);
// 		i++;
// 	}
// 	write(1, "\n", 1);
// 	return (0);
// }
