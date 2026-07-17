/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawaka <takawaka@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/08 19:41:48 by takawaka          #+#    #+#             */
/*   Updated: 2026/07/09 15:47:45 by takawaka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_codexion.h"

long long	get_time_in_ms(void)
{
	struct timeval	tv;
	long long		res;

	if (gettimeofday(&tv, NULL) == -1)
		return (0);
	res = (tv.tv_sec * 1000LL) + (tv.tv_usec / 1000LL);
	return (res);
}

static void	all_free_sub(t_data *data)
{
	free(data->dongle_mutexes);
	free(data->dongle_conds);
	free(data->dongle_queues);
	free(data->dongle_last_free);
	pthread_mutex_destroy(&data->print_mutex);
	pthread_mutex_destroy(&data->state_mutex);
}

void	all_free(t_coder **coder, t_data *data)
{
	int	i;

	if (!data)
		return ;
	i = 0;
	while (i < data->num_coders)
	{
		if (data->dongle_mutexes)
			pthread_mutex_destroy(&data->dongle_mutexes[i]);
		if (data->dongle_conds)
			pthread_cond_destroy(&data->dongle_conds[i]);
		if (data->dongle_queues && data->dongle_queues[i].nodes)
			free(data->dongle_queues[i].nodes);
		i++;
	}
	all_free_sub(data);
	if (coder && *coder)
	{
		free(*coder);
		*coder = NULL;
	}
}
