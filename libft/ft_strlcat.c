/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcat.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/22 19:04:55 by vorhansa          #+#    #+#             */
/*   Updated: 2026/10/22 19:05:03 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlcat(char *dst, const char *src, size_t dstsize)
{
	size_t	i;
	size_t	i2;

	if (!src)
		return (ft_strlen(dst));
	i = -1;
	while (++i < dstsize && *dst)
		dst++;
	if (i == dstsize)
		return (i + ft_strlen(src));
	i2 = 0;
	while (src[i2] != '\0')
	{
		if (i2 <= dstsize - 1 - i)
			*dst++ = src[i2++];
		else
			i2++;
	}
	*dst = '\0';
	return (i + i2);
}
