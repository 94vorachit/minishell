/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/29 17:54:01 by vorhansa          #+#    #+#             */
/*   Updated: 2026/10/29 18:50:39 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/**
 * Checks whether the command name is one of the shell builtins
 * 
 * What it does:
 * * receives the command name as a string, like `"echo"` or `"cd"`
 * * compares it to the list of builtins
 * 
 * returns:
 * * `1` if it is a builtin
 * * `0` otherwise
 */
int	is_builtin(char *cmd)
{
	if (!cmd)
		return (0);
	if (ft_strcmp(cmd, "echo") == 0)
		return (1);
	if (ft_strcmp(cmd, "cd") == 0)
		return (1);
	if (ft_strcmp(cmd, "pwd") == 0)
		return (1);
	if (ft_strcmp(cmd, "export") == 0)
		return (1);
	if (ft_strcmp(cmd, "unset") == 0)
		return (1);
	if (ft_strcmp(cmd, "env") == 0)
		return (1);
	if (ft_strcmp(cmd, "exit") == 0)
		return (1);
	return (0);
}

/**
 * Dispatches to the correct builtin implementation
 * 
 * What it does:
 * * reads `cmd->argv[0]`, which is the command name
 * * compares it with each builtin
 * * calls the matching function
 * * returns `ERROR` if none match
 * 
 * Example:
 * * command: `echo hello`
 * * `cmd->argv[0] == "echo"`
 * * it calls `builtin_echo(data, cmd)`
 */
int	execute_builtin(t_data *data, t_cmd *cmd)
{
	if (ft_strcmp(cmd->argv[0], "echo") == 0)
		return (builtin_echo(data, cmd));
	if (ft_strcmp(cmd->argv[0], "cd") == 0)
		return (builtin_cd(data, cmd));
	if (ft_strcmp(cmd->argv[0], "pwd") == 0)
		return (builtin_pwd(data, cmd));
	if (ft_strcmp(cmd->argv[0], "export") == 0)
		return (builtin_export(data, cmd));
	if (ft_strcmp(cmd->argv[0], "unset") == 0)
		return (builtin_unset(data, cmd));
	if (ft_strcmp(cmd->argv[0], "env") == 0)
		return (builtin_env(data, cmd));
	if (ft_strcmp(cmd->argv[0], "exit") == 0)
		return (builtin_exit(data, cmd));
	return (ERROR);
}
