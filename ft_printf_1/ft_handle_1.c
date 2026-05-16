/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_handle_1.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawaka <takawaka@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/26 14:41:38 by takawaka          #+#    #+#             */
/*   Updated: 2026/05/14 09:24:22 by takawaka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int	handle_c(va_list args)
{
	char	c;

	c = (char)va_arg(args, int);
	return (write(1, &c, 1));
}

int	handle_s(va_list args)
{
	char	*s;
	int		len;

	s = va_arg(args, char *);
	if (!s)
		return (write(1, "(null)", 6));
	len = (int)ft_strlen(s);
	return (write(1, s, len));
}

int	handle_p(va_list args)
{
	unsigned long	nbr;
	void			*temp;
	int				count;
	int				temp2;

	count = 0;
	temp = va_arg(args, void *);
	if (!temp)
		return (write(1, "(nil)", 5));
	nbr = (unsigned long)temp;
	count = write(1, "0x", 2);
	if (count == -1)
		return (-1);
	temp2 = put_nbr_base(nbr, "0123456789abcdef");
	if (temp2 == -1)
		return (-1);
	return (count + temp2);
}

int	handle_d(va_list args)
{
	unsigned long	nbr;
	long			temp;
	int				count;
	int				temp2;

	count = 0;
	temp = (long)va_arg(args, int);
	if (temp < 0)
	{
		temp = -temp;
		count = write(1, "-", 1);
	}
	if (count == -1)
		return (-1);
	nbr = temp;
	temp2 = put_nbr_base(nbr, "0123456789");
	if (temp2 == -1)
		return (-1);
	return (count + temp2);
}

int	handle_i(va_list args)
{
	unsigned long	nbr;
	long			temp;
	int				count;
	int				temp2;

	count = 0;
	temp = (long)va_arg(args, int);
	if (temp < 0)
	{
		temp = -temp;
		count = write(1, "-", 1);
	}
	if (count == -1)
		return (-1);
	nbr = temp;
	temp2 = put_nbr_base(nbr, "0123456789");
	if (temp2 == -1)
		return (-1);
	return (count + temp2);
}
