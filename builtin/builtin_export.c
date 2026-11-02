/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/29 16:58:50 by vorhansa          #+#    #+#             */
/*   Updated: 2026/11/02 17:32:24 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/**
 * Checks whether an environment variable name is valid for `export`.
 * 
 * What it does:
 * 1. Rejects `NULL` or empty strings
 * 2. First character must be a letter or `_`
 * 3. Remaining characters must be letters, digits, or `_`
 */
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
 * Shared validation error handler for invalid export names.
 */
int	export_invalid_name(char *name)
{
	if (!is_valid_export_name(name))
	{
		ft_putstr_fd("export: not a valid identifier: ", STDERR);
		ft_putstr_fd(name, STDERR);
		ft_putstr_fd("\n", STDERR);
		return (ERROR);
	}
	return (SUCCESS);
}

/**
 * Handles arguments like:
 * * `export NAME=value`
 * * `export NAME`
 * 
 * * If there is no `=`, it treats it as a bare exported variable and 
 * stores it in the export-only list.
 * * If there is =, it splits the string into name and value, 
 * validates the name, stores it in the real env list, 
 * and removes it from the export-only list.
 * * It restores the = in the original input before returning.
 */
int	export_set_value(t_data *data, char *arg)
{
	char	*equal;
	char	*name;
	char	*value;

	equal = ft_strchr(arg, '=');
	if (!equal)
	{
		if (export_invalid_name(arg) == ERROR)
			return (ERROR);
		env_set(&data->export, arg, "''");
		return (SUCCESS);
	}
	*equal = '\0';
	name = arg;
	if (export_invalid_name(name) == ERROR)
	{
		*equal = '=';
		return (ERROR);
	}
	value = equal + 1;
	env_set(&data->env, name, value);
	env_unset(&data->export, name);
	*equal = '=';
	return (SUCCESS);
}

/**
 * Prints all variables in sorted alphabetical order.
 * 
 * It sorts both:
 * * normal environment variables
 * * export-only variables
 * 
 * Then prints both lists one after the other.
*/
void	print_sorted_export(t_data *data)
{
	char	**env_list;
	char	**exp_list;
	int		idx;

	env_list = sort_env_alpha(data->env);
	exp_list = sort_env_alpha(data->export);
	if (!env_list && !exp_list)
		return ;
	idx = 0;
	while (env_list && env_list[idx])
	{
		printf("%s\n", env_list[idx]);
		idx++;
	}
	idx = 0;
	while (exp_list && exp_list[idx])
	{
		printf("%s\n", exp_list[idx]);
		idx++;
	}
	free(env_list);
	free(exp_list);
}

/**
 * Entry point for the builtin command.
 * 
 * If there is no argument:
 * * print all exported/sorted items
 * 
 * Otherwise:
 * * process the single provided argument through export_set_value
 * 
 * note : 
 * 
 * _=/home/vorhansa/Documents/my_github/minishell/./minishell
 * This is usually not created by your export builtin. 
 * It is inherited from the parent process or 
 * the shell that launched your program.
 */
int	builtin_export(t_data *data, t_cmd *cmd)
{
	if (!cmd->argv[1])
	{
		print_sorted_export(data);
		return (SUCCESS);
	}
	return (export_set_value(data, cmd->argv[1]));
}
