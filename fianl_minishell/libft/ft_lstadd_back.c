/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstadd_back.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/22 19:02:19 by vorhansa          #+#    #+#             */
/*   Updated: 2026/10/22 19:02:20 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_lstadd_back(t_list **alst, t_list *new)
{
	t_list	*last_elem;

	if (alst && new)
	{
		if (!*alst)
			*alst = new;
		else
		{
			last_elem = ft_lstlast(*alst);
			last_elem->next = new;
		}
	}
}
