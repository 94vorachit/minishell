/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/29 16:58:50 by vorhansa          #+#    #+#             */
/*   Updated: 2026/11/02 14:53:20 by vorhansa         ###   ########.fr       */
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

int	export_invalid_name(char *name)
{
	if (!is_valid_export_name(name))
	{
		ft_putstr_fd("export: not a valid identifier\n", STDERR);
		return (ERROR);
	}
	return (SUCCESS);
}

int	export_set_value(t_data *data, char *arg)
{
	char	*equal;
	char	*name;
	char	*value;

	equal = ft_strchr(arg, '=');
	if (!equal)
		return (export_invalid_name(arg));
	*equal = '\0';
	name = arg;
	if (export_invalid_name(name) == ERROR)
	{
		*equal = '=';
		return (ERROR);
	}
	value = equal + 1;
	env_set(&data->env, name, value);
	*equal = '=';
	return (SUCCESS);
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
	if (!cmd->argv[1])
		return (builtin_env(data, cmd));
	return (export_set_value(data, cmd->argv[1]));
}
