/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rush.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawaka <takawaka@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/04 18:12:03 by takawaka          #+#    #+#             */
/*   Updated: 2026/04/05 20:22:19 by taisuzuk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RUSH_H
# define RUSH_H

# include <fcntl.h>
# include <stdlib.h>
# include <unistd.h>

int		ft_strlen(char *str);
int		ft_strcmp(char *s1, char *s2);
int		print_value(int fd, char c, int *first);
int		check_and_trim(char **num);
int		main(int argc, char **argv);
int		is_numeric(char *str);
int		check_all_units(char *dict, int len);
int		is_key_in_dict(char *path, char *target);
int		print_from_dict(char *path, char *target, int *first);
int		read_key(int fd, char *c, char *buf);
void	handle_below_20(char *str, int size, char *dict, int *first);
void	handle_20_plus(char *str, int size, char *dict, int *first);
void	handle_3_digits(char *str, int size, char *dict, int *first);
void	print_unit(int zero_count, char *dict, int *first);
void	get_chunk(char *dest, char *src, int size);
void	process_chunk(char **str, char *dict, int *len, int *first);
int		solve(char *str, char *dict);

#endif
