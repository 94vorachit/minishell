/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec3.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/09 17:00:32 by vorhansa          #+#    #+#             */
/*   Updated: 2026/11/20 15:00:36 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/**
 * Purpose: function implementation.
 *
 * How it works: runs this function's core steps and returns its status/result.
 */
int	ft_is_directory_path(const char *path)
{
	int	fd;

	fd = open(path, O_RDONLY | O_DIRECTORY);
	if (fd < 0)
		return (0);
	close(fd);
	return (1);
}

/**
 * Saves original stdin/stdout so built-ins can be run with 
 * temporary redirection.
 */
int	ft_backup_std_fds(int *in_fd, int *out_fd)
{
	*in_fd = dup(STDIN_FILENO);
	*out_fd = dup(STDOUT_FILENO);
	if (*in_fd < 0 || *out_fd < 0)
	{
		if (*in_fd >= 0)
			close(*in_fd);
		if (*out_fd >= 0)
			close(*out_fd);
		perror("dup");
		return (ERROR);
	}
	return (SUCCESS);
}

/**
 * Restores stdin/stdout after the built-in command has finished.
 */
void	ft_restore_std_fds(int in_fd, int out_fd)
{
	dup2(in_fd, STDIN_FILENO);
	dup2(out_fd, STDOUT_FILENO);
	close(in_fd);
	close(out_fd);
}

/**
 * Purpose: run shell built-ins safely with redirection.
 * 
 * How it works:
 * 1. duplicate original stdin/stdout
 * 2. apply input/output redirection if needed
 * 3. call `ft_execute_builtin(data, cmd)`
 * 4. restore original stdio
 * 5. return built-in result
 */
int	ft_run_builtin_cmd(t_data *data, t_cmd *cmd)
{
	int	in_fd;
	int	out_fd;
	int	ret;

	if (ft_backup_std_fds(&in_fd, &out_fd) == ERROR)
	{
		data->status = ERROR;
		return (ERROR);
	}
	if (((cmd->infile || cmd->heredoc_fd != -1) && ft_redir_input(cmd) == -1)
		|| (cmd->outfile && ft_redir_output(cmd) == -1))
	{
		ft_restore_std_fds(in_fd, out_fd);
		data->status = ERROR;
		return (ERROR);
	}
	ret = ft_execute_builtin(data, cmd);
	ft_restore_std_fds(in_fd, out_fd);
	data->status = ret;
	return (ret);
}
