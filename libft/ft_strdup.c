/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strdup.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/22 19:04:27 by vorhansa          #+#    #+#             */
/*   Updated: 2026/10/22 19:04:28 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strdup(const char *s1)
{
	size_t	size;
	char	*s2;

	if (!s1)
		return (0);
	size = ft_strlen(s1);
	s2 = malloc(sizeof(*s2) * size + 1);
	if (!s2)
		ft_error(2);
	ft_strlcpy(s2, s1, size + 1);
	return (s2);
}	
