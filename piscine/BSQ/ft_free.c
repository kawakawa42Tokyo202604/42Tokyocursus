#include "ft_header.h"

void	ft_free_map(t_map map)
{
	int	i;

	i = 0;
	while (map.matrix[i])
	{
		free(map.matrix[i]);
		i++;
	}
	free(map.matrix);
}

void	ft_free(char **array_map)
{
	int	i;

	i = 0;
	while (array_map[i])
	{
		free(array_map[i]);
		i++;
	}
	free(array_map);
}
