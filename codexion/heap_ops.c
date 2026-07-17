/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_ops.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawaka <takawaka@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/03 15:43:56 by takawaka          #+#    #+#             */
/*   Updated: 2026/07/09 17:08:38 by takawaka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_codexion.h"

static int	is_higher_priority(t_heap_node a, t_heap_node b)
{
	if (a.priority_key < b.priority_key)
		return (1);
	if (a.priority_key == b.priority_key && a.coder_id < b.coder_id)
		return (1);
	return (0);
}

static void	swap_nodes(t_heap_node *a, t_heap_node *b)
{
	t_heap_node	tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

void	push_queue(t_dongle_queue *queue, int coder_id, long long key)
{
	int	curr;
	int	parent;

	curr = queue->size;
	queue->nodes[curr].coder_id = coder_id;
	queue->nodes[curr].priority_key = key;
	queue->size++;
	while (curr > 0)
	{
		parent = (curr - 1) / 2;
		if (is_higher_priority(queue->nodes[curr], queue->nodes[parent]))
		{
			swap_nodes(&queue->nodes[curr], &queue->nodes[parent]);
			curr = parent;
		}
		else
			break ;
	}
}

void	pop_queue(t_dongle_queue *queue)
{
	int	curr;
	int	child;

	if (queue->size <= 0)
		return ;
	queue->size--;
	queue->nodes[0] = queue->nodes[queue->size];
	curr = 0;
	while ((curr * 2 + 1) < queue->size)
	{
		child = curr * 2 + 1;
		if (child + 1 < queue->size \
			&& is_higher_priority(queue->nodes[child + 1], queue->nodes[child]))
			child++;
		if (is_higher_priority(queue->nodes[child], queue->nodes[curr]))
		{
			swap_nodes(&queue->nodes[curr], &queue->nodes[child]);
			curr = child;
		}
		else
			break ;
	}
}
