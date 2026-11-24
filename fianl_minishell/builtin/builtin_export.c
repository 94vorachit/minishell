/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/20 14:44:03 by vorhansa          #+#    #+#             */
/*   Updated: 2026/11/20 18:59:50 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/**
 * Purpose: export a variable name without an assignment.
 *
 * How it works:
 * 1. validates the variable name
 * 2. adds it to export-only storage if it is not already exported
 * 3. does not create or overwrite a value in the environment
 */
int	ft_export_set_name(t_data *data, char *arg)
{
	if (ft_export_invalid_name(arg, NULL) == ERROR)
		return (ERROR);
	if (!ft_env_find(data->env, arg)
		&& !ft_export_find_name(data->export, arg))
		ft_env_add_last(&data->export, ft_env_new(arg));
	return (SUCCESS);
}

/**
 * Purpose: export a variable with a NAME=value assignment.
 *
 * How it works:
 * 1. temporarily separates the name from the value
 * 2. validates the variable name
 * 3. stores or updates it in the environment
 * 4. removes its export-only entry and restores the '=' character
 */
int	ft_export_set_assignment(t_data *data, char *arg, char *equal)
{
	char	*name;
	char	*value;

	*equal = '\0';
	name = arg;
	if (ft_export_invalid_name(name, equal) == ERROR)
	{
		*equal = '=';
		return (ERROR);
	}
	value = equal + 1;
	ft_env_set(&data->env, name, value);
	ft_env_unset(&data->export, name);
	*equal = '=';
	return (SUCCESS);
}

/**
 * Purpose: dispatch one export argument to the correct handler.
 *
 * How it works:
 * 1. searches for '=' in the argument
 * 2. calls the bare-name handler when '=' is absent
 * 3. calls the assignment handler when '=' is present
 */
int	ft_export_set_value(t_data *data, char *arg)
{
	char	*equal;

	equal = ft_strchr(arg, '=');
	if (!equal)
		return (ft_export_set_name(data, arg));
	return (ft_export_set_assignment(data, arg, equal));
}

/**
 * Entry point for the builtin command.
 * 
 * If there is no argument:
 * * print all exported/sorted items
 * 
 * Otherwise:
 * * process the single provided argument through ft_export_set_value
 * 
 * note : 
 * 
 * _=/home/vorhansa/Documents/my_github/minishell/./minishell
 * This is usually not created by your export builtin. 
 * It is inherited from the parent process or 
 * the shell that launched your program.
 */
int	ft_builtin_export(t_data *data, t_cmd *cmd)
{
	int	i;
	int	res;
	int	status;

	if (!cmd->argv[1])
	{
		ft_print_sorted_export(data);
		return (SUCCESS);
	}
	i = 1;
	status = SUCCESS;
	while (cmd->argv[i])
	{
		res = ft_export_set_value(data, cmd->argv[i]);
		if (res == ERROR)
			status = ERROR;
		i++;
	}
	return (status);
}
