/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation2.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawaka <takawaka@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 17:13:09 by takawaka          #+#    #+#             */
/*   Updated: 2026/07/09 17:40:56 by takawaka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_codexion.h"

void	print_log(t_coder *coder, char *msg)
{
	pthread_mutex_lock(&coder->data->print_mutex);
	pthread_mutex_lock(&coder->data->state_mutex);
	if (!coder->data->stop_flag || strcmp(msg, "burned out") == 0)
	{
		printf("[%lld] %d %s\n", get_time_in_ms() - coder->data->start_time,
			coder->id, msg);
	}
	pthread_mutex_unlock(&coder->data->state_mutex);
	pthread_mutex_unlock(&coder->data->print_mutex);
}

void	stop_simulation(t_data *data)
{
	int	j;

	pthread_mutex_lock(&data->state_mutex);
	data->stop_flag = 1;
	pthread_mutex_unlock(&data->state_mutex);
	j = 0;
	while (j < data->num_coders)
	{
		pthread_mutex_lock(&data->dongle_mutexes[j]);
		pthread_cond_broadcast(&data->dongle_conds[j]);
		pthread_mutex_unlock(&data->dongle_mutexes[j]);
		j++;
	}
}

int	wait_dongle(t_coder *coder, int idx, long long key)
{
	t_data			*data;
	t_dongle_queue	*q;
	long long		rem;

	data = coder->data;
	q = &data->dongle_queues[idx];
	pthread_mutex_lock(&data->dongle_mutexes[idx]);
	push_queue(q, coder->id, key);
	while (q->nodes[0].coder_id != coder->id && !data->stop_flag)
		pthread_cond_wait(&data->dongle_conds[idx], &data->dongle_mutexes[idx]);
	if (data->stop_flag)
		return (pthread_mutex_unlock(&data->dongle_mutexes[idx]), 1);
	rem = data->cooldown - (get_time_in_ms() - data->dongle_last_free[idx]);
	while (rem > 0 && !data->stop_flag)
	{
		pthread_mutex_unlock(&data->dongle_mutexes[idx]);
		usleep(rem * 1000);
		pthread_mutex_lock(&data->dongle_mutexes[idx]);
		while (q->nodes[0].coder_id != coder->id && !data->stop_flag)
			pthread_cond_wait(&data->dongle_conds[idx],
				&data->dongle_mutexes[idx]);
		rem = data->cooldown - (get_time_in_ms() - data->dongle_last_free[idx]);
	}
	return (pthread_mutex_unlock(&data->dongle_mutexes[idx]), data->stop_flag);
}

int	take_dongles(t_coder *coder)
{
	long long	key;
	int			low;
	int			high;

	if (coder->data->is_edf)
		key = coder->last_compile_start + coder->data->time_to_burn;
	else
		key = get_time_in_ms();
	low = coder->left_dongle;
	high = coder->right_dongle;
	if (low > high)
	{
		low = coder->right_dongle;
		high = coder->left_dongle;
	}
	if (wait_dongle(coder, low, key))
		return (1);
	print_log(coder, "has taken a dongle");
	if (wait_dongle(coder, high, key))
		return (1);
	print_log(coder, "has taken a dongle");
	return (0);
}

void	release_dongles(t_coder *coder)
{
	t_data	*data;
	int		i;
	int		idx;

	data = coder->data;
	i = 0;
	while (i < 2)
	{
		if (i == 0)
			idx = coder->left_dongle;
		else
			idx = coder->right_dongle;
		pthread_mutex_lock(&data->dongle_mutexes[idx]);
		pop_queue(&data->dongle_queues[idx]);
		data->dongle_last_free[idx] = get_time_in_ms();
		pthread_cond_broadcast(&data->dongle_conds[idx]);
		pthread_mutex_unlock(&data->dongle_mutexes[idx]);
		i++;
	}
}
