/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/29 16:58:50 by vorhansa          #+#    #+#             */
/*   Updated: 2026/10/29 20:28:49 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	is_valid_export_name(char *name)
{
	int	idx;

	if (!name || !*name)
		return (0);
	if (!(ft_isalpha((unsigned char)name[0]) || name[0] == '_'))
		return (0);
	idx = 1;
	while (name[idx])
	{
		if (!(ft_isalnum((unsigned char)name[idx]) || name[idx] == '_'))
			return (0);
		idx++;
	}
	return (1);
}

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
	{
		if (!is_valid_export_name(cmd->argv[1]))
		{
			ft_putstr_fd("export: not a valid identifier\n", STDERR);
			return (ERROR);
		}
		return (SUCCESS);
	}
	*equal = '\0';
	name = cmd->argv[1];
	if (!is_valid_export_name(name))
	{
		*equal = '=';
		ft_putstr_fd("export: not a valid identifier\n", STDERR);
		return (ERROR);
	}
	value = equal + 1;
	env_set(&data->env, name, value);
	*equal = '=';
	return (SUCCESS);
}
