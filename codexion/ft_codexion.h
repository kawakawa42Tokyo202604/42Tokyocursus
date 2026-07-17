/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_codexion.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawaka <takawaka@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/03 12:21:24 by takawaka          #+#    #+#             */
/*   Updated: 2026/07/09 17:41:06 by takawaka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_CODEXION_H
# define FT_CODEXION_H

# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/time.h>
# include <unistd.h>

typedef struct s_heap_node
{
	int				coder_id;
	long long		priority_key;
}					t_heap_node;

typedef struct s_dongle_queue
{
	t_heap_node		*nodes;
	int				size;
}					t_dongle_queue;

typedef struct s_data
{
	int				num_coders;
	int				time_to_burn;
	int				time_to_comp;
	int				time_to_dbg;
	int				time_to_ref;
	int				req_compiles;
	int				cooldown;
	int				is_edf;
	long long		start_time;
	int				stop_flag;
	long long		*dongle_last_free;
	t_dongle_queue	*dongle_queues;
	pthread_cond_t	*dongle_conds;
	pthread_mutex_t	print_mutex;
	pthread_mutex_t	state_mutex;
	pthread_mutex_t	*dongle_mutexes;
}					t_data;

typedef struct s_coder
{
	int				id;
	int				compile_count;
	long long		last_compile_start;
	int				left_dongle;
	int				right_dongle;
	pthread_t		thread_id;
	t_data			*data;
}					t_coder;

int					init(t_data *data, char *av[], t_coder **coders);
long long			get_time_in_ms(void);
void				all_free(t_coder **coder, t_data *data);
void				push_queue(t_dongle_queue *queue, int coder_id,
						long long key);
void				pop_queue(t_dongle_queue *queue);
int					run_simulation(t_coder *coders);

void				print_log(t_coder *coder, char *msg);
void				stop_simulation(t_data *data);
int					wait_dongle(t_coder *coder, int idx, long long key);
int					take_dongles(t_coder *coder);
void				release_dongles(t_coder *coder);
int					wait_dongle(t_coder *coder, int idx, long long key);

#endif
