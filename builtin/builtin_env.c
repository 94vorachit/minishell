/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_env.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/29 16:58:40 by vorhansa          #+#    #+#             */
/*   Updated: 2026/10/29 17:52:18 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/**
 * Print each environment node as `name=value`:
 */
int	builtin_env(t_data *data, t_cmd *cmd)
{
	t_env	*cur;

	(void)cmd;
	cur = data->env;
	while (cur)
	{
		if (cur->value && ft_strchr(cur->value, '='))
			printf("%s\n", cur->value);
		cur = cur->next;
	}
	return (SUCCESS);
}
