/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_unset.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/29 16:58:59 by vorhansa          #+#    #+#             */
/*   Updated: 2026/10/29 17:52:42 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/**
 * Remove matching entries:
 */
int	builtin_unset(t_data *data, t_cmd *cmd)
{
	int	i;

	i = 1;
	while (cmd->argv[i])
	{
		env_unset(&data->env, cmd->argv[i]);
		i++;
	}
	return (SUCCESS);
}
