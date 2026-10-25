/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/22 19:03:10 by vorhansa          #+#    #+#             */
/*   Updated: 2026/10/22 19:03:11 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memchr(const void *s, int c, size_t n)
{
	size_t			i;
	unsigned char	*tmp;

	i = 0;
	while (n - i)
	{
		if (((unsigned char *)s)[i] == (unsigned char)c)
		{
			tmp = (unsigned char *)(s + i);
			return (tmp);
		}
		i++;
	}
	return (0);
}
