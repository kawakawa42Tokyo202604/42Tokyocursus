/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawaka <takawaka@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/26 17:55:34 by takawaka          #+#    #+#             */
/*   Updated: 2026/05/13 09:26:25 by takawaka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_PRINTF_H
# define FT_PRINTF_H

# include <stdarg.h>
# include <stddef.h>
# include <stdlib.h>
# include <unistd.h>

size_t	ft_strlen(const char *s);
int		ft_printf(const char *format, ...)__attribute__((format(printf, 1, 2)));
int		handler(char c, va_list args);
int		handle_c(va_list args);
int		handle_s(va_list args);
int		handle_p(va_list args);
int		handle_d(va_list args);
int		handle_i(va_list args);
int		handle_u(va_list args);
int		handle_little_x(va_list args);
int		handle_big_x(va_list args);
int		put_nbr_base(unsigned long nbr, char *base);
int		write_error(int count, int value);
int		get_nbr_len(unsigned long nbr, unsigned long base_len);

#endif
