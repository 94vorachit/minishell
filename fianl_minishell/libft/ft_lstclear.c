/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstclear.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/22 19:02:33 by vorhansa          #+#    #+#             */
/*   Updated: 2026/10/22 19:02:34 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_lstclear(t_list **lst, void (*del)(void *))
{
	t_list	*elem_to_del;
	t_list	*tmp;

	if (lst && del)
	{
		elem_to_del = *lst;
		while (elem_to_del)
		{
			tmp = elem_to_del->next;
			del(elem_to_del->content);
			free(elem_to_del);
			elem_to_del = tmp;
		}
		*lst = NULL;
	}
}
