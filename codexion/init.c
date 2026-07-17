/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawaka <takawaka@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/03 12:33:09 by takawaka          #+#    #+#             */
/*   Updated: 2026/07/08 19:40:47 by takawaka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_codexion.h"

static int	init_dongles(t_data *data)
{
	int	i;

	data->dongle_mutexes = malloc(sizeof(pthread_mutex_t) * data->num_coders);
	data->dongle_conds = malloc(sizeof(pthread_cond_t) * data->num_coders);
	data->dongle_queues = malloc(sizeof(t_dongle_queue) * data->num_coders);
	data->dongle_last_free = malloc(sizeof(long long) * data->num_coders);
	if (!data->dongle_mutexes || !data->dongle_conds || !data->dongle_queues
		|| !data->dongle_last_free)
		return (1);
	i = 0;
	while (i < data->num_coders)
	{
		if (pthread_mutex_init(&data->dongle_mutexes[i], NULL) != 0
			|| pthread_cond_init(&data->dongle_conds[i], NULL) != 0)
			return (1);
		data->dongle_queues[i].nodes = malloc(sizeof(t_heap_node)
				* data->num_coders);
		if (!data->dongle_queues[i].nodes)
			return (1);
		data->dongle_queues[i].size = 0;
		data->dongle_last_free[i] = data->start_time;
		i++;
	}
	return (0);
}

static int	init_data(t_data *data, char *av[])
{
	data->num_coders = atoi(av[1]);
	data->time_to_burn = atoi(av[2]);
	data->time_to_comp = atoi(av[3]);
	data->time_to_dbg = atoi(av[4]);
	data->time_to_ref = atoi(av[5]);
	data->req_compiles = atoi(av[6]);
	data->cooldown = atoi(av[7]);
	data->is_edf = (strcmp(av[8], "edf") == 0);
	data->start_time = get_time_in_ms();
	data->stop_flag = 0;
	if (pthread_mutex_init(&(data->print_mutex), NULL) != 0
		|| pthread_mutex_init(&(data->state_mutex), NULL) != 0)
		return (1);
	if (init_dongles(data) != 0)
		return (1);
	return (0);
}

static t_coder	*malloc_coders(t_data *data)
{
	t_coder	*coders;
	int		i;

	coders = malloc(sizeof(t_coder) * data->num_coders);
	if (!coders)
		return (NULL);
	i = 0;
	while (i < data->num_coders)
	{
		coders[i].id = i + 1;
		coders[i].compile_count = 0;
		coders[i].last_compile_start = data->start_time;
		coders[i].left_dongle = i;
		coders[i].right_dongle = (i + 1) % data->num_coders;
		coders[i].data = data;
		i++;
	}
	return (coders);
}

int	init(t_data *data, char *av[], t_coder **coders)
{
	if (init_data(data, av) == 1)
		return (1);
	*coders = malloc_coders(data);
	if (!*coders)
		return (1);
	return (printf("OK\n"), 0);
}
