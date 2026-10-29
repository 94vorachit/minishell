/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/29 16:58:50 by vorhansa          #+#    #+#             */
/*   Updated: 2026/10/29 17:53:03 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/**
 * This one is a bit more involved.
 * 
 * Cases:
 * * `export` with no args → print environment variables
 * * `export NAME=value` → add/update env var
 * * `export NAME` → if it exists, do nothing; otherwise add NAME= maybe or 
 * ignore depending on Bash handling
*/
int	builtin_export(t_data *data, t_cmd *cmd)
{
	char	*name;
	char	*value;
	char	*equal;

	if (!cmd->argv[1])
		return (builtin_env(data, cmd));
	equal = ft_strchr(cmd->argv[1], '=');
	if (!equal)
		return (SUCCESS);
	*equal = '\0';
	name = cmd->argv[1];
	value = equal + 1;
	env_set(&data->env, name, value);
	return (SUCCESS);
}
