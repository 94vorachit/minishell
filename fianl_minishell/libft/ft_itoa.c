/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/22 19:02:12 by vorhansa          #+#    #+#             */
/*   Updated: 2026/10/22 19:02:13 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static size_t	ft_len_int(long int n)
{
	size_t	size;

	if (n == 0)
		return (1);
	size = 0;
	if (n < 0)
	{
		n *= -1;
		size++;
	}
	while (n)
	{
		n /= 10;
		size += 1;
	}
	return (size);
}

char	*ft_itoa(int n)
{
	size_t		size;
	char		*str;
	long int	nb;

	size = ft_len_int(n);
	str = malloc(sizeof(*str) * size + 1);
	if (!str)
		return (0);
	nb = n;
	if (n < 0)
		nb = nb * -1;
	str[size] = '\0';
	while (size > 0)
	{
		str[size - 1] = nb % 10 + 48;
		nb /= 10;
		size--;
	}
	if (n < 0)
		str[0] = '-';
	return (str);
}
