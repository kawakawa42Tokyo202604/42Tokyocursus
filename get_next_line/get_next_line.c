/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawaka <takawaka@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/27 09:35:40 by takawaka          #+#    #+#             */
/*   Updated: 2026/05/15 09:38:08 by takawaka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

char	*get_next_line(int fd)
{
	static char	*save;
	char		*buf;
	ssize_t		size;

	size = 1;
	buf = (char *)malloc(sizeof(char) * BUFFER_SIZE + 1);
	if (is_error(fd, buf))
	{
		save = all_free(buf, save);
		return (save);
	}
	while (is_newline(save) == -1 && size != 0)
	{
		size = read(fd, buf, BUFFER_SIZE);
		if (size == -1)
		{
			save = all_free(buf, save);
			return (save);
		}
		buf[size] = '\0';
		save = ft_strjoinfree(buf, save);
	}
	free(buf);
	return (ft_strjointrim(&save));
}

int	ft_if_null_free(char **s)
{
	if (!s || !*s || **s == '\0')
	{
		if (s && *s)
		{
			free(*s);
			*s = NULL;
		}
		return (1);
	}
	return (0);
}

char	*ft_strjointrim(char **save)
{
	ssize_t	i;
	ssize_t	len;
	char	*res;
	char	*temp;

	if (ft_if_null_free(save) == 1)
		return (NULL);
	len = is_newline(*save);
	if (len == -1)
		len = ft_strlengnl(*save);
	else
		++len;
	res = (char *)malloc(len + 1);
	if (!res)
		return (NULL);
	i = -1;
	while (++i < len && (*save)[i])
		res[i] = (*save)[i];
	res[i] = '\0';
	temp = ft_strdupgnl(*save + i);
	free(*save);
	*save = temp;
	if (*save && **save == '\0')
		*save = ft_cleangnl(*save);
	return (res);
}

char	*ft_strjoinfree(char *buf, char *save)
{
	char	*res;
	ssize_t	i;
	ssize_t	j;

	res = (char *)malloc(ft_strlengnl(save) + ft_strlengnl(buf) + 1);
	if (!res)
	{
		free(save);
		return (NULL);
	}
	i = 0;
	while (save && save[i])
	{
		res[i] = save[i];
		++i;
	}
	j = 0;
	while (buf && buf[j])
	{
		res[i + j] = buf[j];
		++j;
	}
	res[i + j] = '\0';
	free(save);
	return (res);
}

int	is_error(int fd, char *buf)
{
	char	dummy;

	if (fd < 0 || !buf || BUFFER_SIZE <= 0)
		return (1);
	if (read(fd, &dummy, 0) == -1)
		return (1);
	return (0);
}
