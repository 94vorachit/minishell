/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_unset.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/20 14:44:14 by vorhansa          #+#    #+#             */
/*   Updated: 2026/11/20 16:02:12 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/**
 * Remove matching entries:
 */
int	ft_builtin_unset(t_data *data, t_cmd *cmd)
{
	int	i;

	i = 1;
	while (cmd->argv[i])
	{
		ft_env_unset(&data->env, cmd->argv[i]);
		i++;
	}
	return (SUCCESS);
}
