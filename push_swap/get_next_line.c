#include "push_swap.h"

static char	*read_and_save(int fd, char *save)
{
	char	*buff;
	ssize_t	read_bytes;

	buff = (char *)malloc(sizeof(char) * (BUFFER_SIZE + 1));
	if (!buff)
		return (ft_free(save));
	while (1)
	{
		read_bytes = read(fd, buff, BUFFER_SIZE);
		if (read_bytes == -1)
		{
			free(buff);
			return (ft_free(save));
		}
		if (read_bytes == 0)
			break ;
		buff[read_bytes] = '\0';
		save = ft_strjoin(save, buff);
		if (!save)
			return (ft_free(buff));
		if (ft_strchr(buff, END))
			break ;
	}
	free(buff);
	return (save);
}

char	*get_next_line(int fd)
{
	static char	*save[OPEN_MAX];
	char		*res;

	if (fd < 0 || fd >= OPEN_MAX || BUFFER_SIZE <= 0)
		return (NULL);
	if (!save[fd] || !ft_strchr(save[fd], END))
		save[fd] = read_and_save(fd, save[fd]);
	if (!save[fd])
		return (NULL);
	res = get_line_from_save(save[fd]);
	if (!res)
	{
		free(save[fd]);
		save[fd] = NULL;
		return (NULL);
	}
	save[fd] = update_save(save[fd]);
	return (res);
}

char	*get_line_from_save(char *save)
{
	int		i;
	char	*str;

	if (!save || !save[0])
		return (NULL);
	i = 0;
	while (save[i] && save[i] != END)
		i++;
	str = (char *)malloc(sizeof(char) * (i + 1 + (save[i] == END)));
	if (!str)
		return (NULL);
	i = 0;
	while (save[i] && save[i] != END)
	{
		str[i] = save[i];
		i++;
	}
	if (save[i] == END)
		str[i++] = END;
	str[i] = '\0';
	return (str);
}

char	*update_save(char *save)
{
	int		i;
	int		j;
	char	*str;

	i = 0;
	while (save[i] && save[i] != END)
		i++;
	if (!save[i] || !save[i + 1])
		return (ft_free(save));
	str = (char *)malloc(sizeof(char) * (ft_strlen(save) - i));
	if (!str)
		return (ft_free(save));
	i++;
	j = 0;
	while (save[i])
		str[j++] = save[i++];
	str[j] = '\0';
	free(save);
	return (str);
}
