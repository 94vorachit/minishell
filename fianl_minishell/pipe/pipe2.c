/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe2.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/19 19:35:00 by vorhansa          #+#    #+#             */
/*   Updated: 2026/11/20 17:07:23 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/**
 * Purpose: execute a resolved external path or exit with a shell error code.
 *
 * How it works:
 * 1. checks whether the resolved path points to a directory
 * 2. if it is a directory, prints `Is a directory`, frees resources, exits 126
 * 3. otherwise calls `execve(path, argv, envp)`
 * 4. if `execve` fails, prints `perror(path)`, frees resources, exits 126
 *
 * This function never returns in normal flow.
 */
void	ft_exec_path_or_exit(char *path, t_cmd *cur, char **envp)
{
	if (ft_is_directory_path(path))
	{
		ft_putstr_fd("minishell: ", STDERR);
		ft_putstr_fd(path, STDERR);
		ft_putstr_fd(": Is a directory\n", STDERR);
		free(path);
		ft_free_split(envp);
		exit(126);
	}
	execve(path, cur->argv, envp);
	perror(path);
	free(path);
	ft_free_split(envp);
	exit(126);
}

/**
 * Purpose: prepare and execute an external command inside a pipeline child.
 * 
 * How it works:
 * 1. resolves the command path from PATH or direct slash form
 * 2. if path is missing, prints command lookup error and exits 127
 * 3. converts shell environment list into `envp` for `execve`
 * 4. if env conversion fails, frees path and exits 126
 * 5. delegates to `ft_exec_path_or_exit` for final execution/error handling
 * 
 * Exit codes follow common shell behavior:
 * 127 for not found, 126 for execution failure.
 */
void	ft_run_external_child_cmd(t_data *data, t_cmd *cur)
{
	char	*path;
	char	**envp;

	path = ft_find_exec_path(data->env, cur->argv[0]);
	if (!path)
	{
		ft_print_exec_error(cur->argv[0]);
		exit(127);
	}
	envp = ft_env_to_array(data->env);
	if (!envp)
	{
		free(path);
		exit(126);
	}
	ft_exec_path_or_exit(path, cur, envp);
}
