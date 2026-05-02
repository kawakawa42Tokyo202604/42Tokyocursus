/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*    ft_str_is_alpha.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawaka <takawaka@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/25 14:23:16 by takawaka          #+#    #+#             */
/*   Updated: 2026/03/25 15:12:09 by takawaka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// #include <unistd.h>

int	ft_str_is_alpha(char *str)
{
	while (*str != '\0')
	{
		if (!(('a' <= *str && *str <= 'z') || ('A' <= *str && *str <= 'Z')))
		{
			return (0);
		}
		++str;
	}
	return (1);
}

// int	main(void)
// {
// 	char	str[30] = "bananan";
// 	int		i;
// 	char	c;

// 	i = ft_str_is_alpha(str);
// 	c = i + '0';
// 	write(1, &c, 1);
// }
