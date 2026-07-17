/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawaka <takawaka@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 12:53:28 by takawaka          #+#    #+#             */
/*   Updated: 2026/07/14 16:52:48 by takawaka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_codexion.h"

static int	is_digit_len(char *s)
{
	int	i;

	i = 0;
	while (*s && (*s == ' ' || (9 <= *s && *s <= 13)))
		++s;
	if (*s == '+')
		++s;
	if (*s == '-' || *s == '\0')
		return (0);
	while ('0' <= *s && *s <= '9')
	{
		++s;
		++i;
	}
	if (*s)
		return (0);
	return (i);
}

static int	is_overflow(char *s)
{
	long	val;

	val = 0;
	while (*s && (*s == ' ' || (9 <= *s && *s <= 13)))
		++s;
	if (*s == '+')
		++s;
	while ('0' <= *s && *s <= '9')
	{
		val = val * 10 + (*s - '0');
		if (val > 2147483647)
			return (1);
		++s;
	}
	return (0);
}

static int	is_error(int ac, char *av[])
{
	int	i;

	if (ac != 9)
		return (printf("Invalid syntax\n"), 1);
	i = 1;
	while (i <= 7)
	{
		if (!is_digit_len(av[i]) || is_digit_len(av[i]) > 11
			|| is_overflow(av[i]))
			return (printf("Invalid syntax\n"), 1);
		++i;
	}
	if (strcmp(av[8], "fifo") != 0 && strcmp(av[8], "edf") != 0)
		return (printf("Invalid syntax\n"), 1);
	return (0);
}

int	main(int ac, char *av[])
{
	t_data	data;
	t_coder	*coders;

	if (is_error(ac, av))
		return (1);
	if (init(&data, av, &coders) != 0)
		return (printf("Error: Initialization failed\n"), 1);
	if (!coders)
		return (all_free(&coders, &data), 1);
	run_simulation(coders);
	all_free(&coders, &data);
	return (0);
}
