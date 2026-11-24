/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memmove.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/22 19:03:29 by vorhansa          #+#    #+#             */
/*   Updated: 2026/10/22 19:03:30 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memmove(void *dst, const void *src, size_t len)
{
	size_t	i;

	if (dst == src)
		return (dst);
	if (!dst && !src)
		return (0);
	i = 0;
	if ((char *)dst > (char *)src)
		while (len--)
			((char *)dst)[len] = ((char *)src)[len];
	else
	{
		while (len--)
		{
			((char *)dst)[i] = ((char *)src)[i];
			i++;
		}
	}
	return (dst);
}
