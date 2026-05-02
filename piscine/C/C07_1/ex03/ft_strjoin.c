/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strjoin.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawaka <takawaka@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/31 11:30:01 by takawaka          #+#    #+#             */
/*   Updated: 2026/04/05 10:13:55 by takawaka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>

int		get_len(int size, char **strs, char *sep);
void	ft_strcat(char **strs, char *sep, char *res, int size);
int		ft_strlen(char *str);
char	*ft_strjoin(int size, char **strs, char *sep);

int	ft_strlen(char *str)
{
	int	i;

	i = 0;
	while (str[i])
		++i;
	return (i);
}

int	get_len(int size, char **strs, char *sep)
{
	int	len;
	int	i;
	int	sep_len;

	if (size <= 0)
		return (0);
	len = 0;
	i = 0;
	sep_len = ft_strlen(sep);
	while (i < size)
	{
		len = len + ft_strlen(strs[i]);
		if (i < size - 1)
			len = len + sep_len;
		++i;
	}
	return (len);
}

char	*ft_strjoin(int size, char **strs, char *sep)
{
	char	*res;

	if (size <= 0)
	{
		res = (char *)malloc(sizeof(char) * 1);
		res[0] = '\0';
		return (res);
	}
	res = (char *)malloc(sizeof(char) * (get_len(size, strs, sep) + 1));
	if (!res)
		return ((void *)0);
	ft_strcat(strs, sep, res, size);
	return (res);
}

void	ft_strcat(char **strs, char *sep, char *res, int size)
{
	int	i;
	int	j;
	int	k;

	i = 0;
	k = 0;
	while (i < size)
	{
		j = 0;
		while (strs[i][j] != '\0')
		{
			res[k] = strs[i][j];
			++j;
			++k;
		}
		if (i < size - 1)
		{
			j = 0;
			while (sep[j] != '\0')
				res[k++] = sep[j++];
		}
		++i;
	}
	res[k] = '\0';
}

// #include <stdio.h>

// int	main(void)
// {
// 	char	*strs[] = {"Hello", "World", "42"};
// 	char	*sep;
// 	char	*res;
// 	int		size;

// 	sep = " , ";
// 	size = 3;
// 	res = ft_strjoin(size, strs, sep);
// 	if (res)
// 	{
// 		printf("%s\n", res);
// 		free(res);
// 	}
// 	return (0);
// }
