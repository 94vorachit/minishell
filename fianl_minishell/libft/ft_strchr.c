/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/22 19:04:19 by vorhansa          #+#    #+#             */
/*   Updated: 2026/10/22 19:04:20 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strchr(const char *s, int c)
{
	while (*s)
	{
		if (*s && *s == (char)c)
			return ((char *)s);
		s++;
	}
	if (*s && *s == (char)c)
		return ((char *)s);
	else
		return (0);
}
