/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawaka <takawaka@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/08 15:59:06 by takawaka          #+#    #+#             */
/*   Updated: 2026/07/09 17:38:23 by takawaka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_codexion.h"

static void	execute_cycle(t_coder *coder, t_data *data)
{
	pthread_mutex_lock(&data->state_mutex);
	coder->last_compile_start = get_time_in_ms();
	pthread_mutex_unlock(&data->state_mutex);
	print_log(coder, "is compiling");
	usleep(data->time_to_comp * 1000);
	pthread_mutex_lock(&data->state_mutex);
	coder->compile_count++;
	pthread_mutex_unlock(&data->state_mutex);
	release_dongles(coder);
	print_log(coder, "is debugging");
	usleep(data->time_to_dbg * 1000);
	print_log(coder, "is refactoring");
	usleep(data->time_to_ref * 1000);
}

static void	*coder_routine(void *arg)
{
	t_coder	*coder;
	t_data	*data;

	coder = (t_coder *)arg;
	data = coder->data;
	if (data->num_coders == 1)
	{
		print_log(coder, "has taken a dongle");
		return (usleep(data->time_to_burn * 1000), NULL);
	}
	if (coder->id % 2 == 0)
		usleep(1000);
	while (!data->stop_flag)
	{
		if (take_dongles(coder))
			break ;
		execute_cycle(coder, data);
	}
	return (NULL);
}

static int	check_coders(t_coder *coders, t_data *data, int *all_done)
{
	int	i;

	i = 0;
	while (i < data->num_coders)
	{
		pthread_mutex_lock(&data->state_mutex);
		if (get_time_in_ms()
			- coders[i].last_compile_start > data->time_to_burn)
		{
			pthread_mutex_unlock(&data->state_mutex);
			print_log(&coders[i], "burned out");
			stop_simulation(data);
			return (1);
		}
		if (data->req_compiles > 0
			&& coders[i].compile_count < data->req_compiles)
			*all_done = 0;
		pthread_mutex_unlock(&data->state_mutex);
		i++;
	}
	return (0);
}

static void	*monitor_routine(void *arg)
{
	t_coder	*coders;
	t_data	*data;
	int		all_done;

	coders = (t_coder *)arg;
	data = coders[0].data;
	while (!data->stop_flag)
	{
		all_done = 1;
		if (check_coders(coders, data, &all_done))
			return (NULL);
		if (data->req_compiles > 0 && all_done)
		{
			stop_simulation(data);
			return (NULL);
		}
		usleep(1000);
	}
	return (NULL);
}

int	run_simulation(t_coder *coders)
{
	t_data		*data;
	pthread_t	monitor_tid;
	pthread_t	*coder_tids;
	int			i;

	data = coders->data;
	coder_tids = malloc(sizeof(pthread_t) * data->num_coders);
	if (!coder_tids)
		return (1);
	i = 0;
	while (i < data->num_coders)
	{
		if (pthread_create(&coder_tids[i], NULL, coder_routine,
				&coders[i]) != 0)
			return (free(coder_tids), 1);
		i++;
	}
	if (pthread_create(&monitor_tid, NULL, monitor_routine, coders) != 0)
		return (free(coder_tids), 1);
	i = 0;
	while (i < data->num_coders)
		pthread_join(coder_tids[i++], NULL);
	pthread_join(monitor_tid, NULL);
	free(coder_tids);
	return (0);
}
