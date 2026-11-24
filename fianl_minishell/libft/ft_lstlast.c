/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstlast.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/22 19:02:49 by vorhansa          #+#    #+#             */
/*   Updated: 2026/10/22 19:02:50 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

t_list	*ft_lstlast(t_list *lst)
{
	t_list	*last_elem;

	if (!lst)
		return (0);
	last_elem = lst;
	while (last_elem->next != NULL)
		last_elem = last_elem->next;
	return (last_elem);
}
