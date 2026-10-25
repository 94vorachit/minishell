/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_substr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/22 19:06:19 by vorhansa          #+#    #+#             */
/*   Updated: 2026/10/22 19:06:20 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	size_t	i;
	char	*dst;

	i = 0;
	if (!s)
		return (0);
	if (start + i < ft_strlen(s))
		while (s[start + i] && i < len)
			i++;
	dst = malloc(sizeof(*dst) * i + 1);
	if (!dst)
		ft_error(2);
	dst[i] = '\0';
	while (i--)
		dst[i] = s[start + i];
	return (dst);
}
