/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec3.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/09 17:00:32 by vorhansa          #+#    #+#             */
/*   Updated: 2026/11/09 17:01:22 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/**
 * Saves original stdin/stdout so built-ins can be run with 
 * temporary redirection.
 */
static int	backup_std_fds(int *in_fd, int *out_fd)
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
static void	restore_std_fds(int in_fd, int out_fd)
{
	dup2(in_fd, STDIN_FILENO);
	dup2(out_fd, STDOUT_FILENO);
	close(in_fd);
	close(out_fd);
}

/**
 * Executes a built-in command with a temporary dup of stdin/stdout.
 */
int	run_builtin_cmd(t_data *data, t_cmd *cmd)
{
	int	in_fd;
	int	out_fd;
	int	ret;

	if (backup_std_fds(&in_fd, &out_fd) == ERROR)
	{
		data->status = ERROR;
		return (ERROR);
	}
	if (((cmd->infile || cmd->heredoc_fd != -1) && redirect_input(cmd) == -1)
		|| (cmd->outfile && redirect_output(cmd) == -1))
	{
		restore_std_fds(in_fd, out_fd);
		data->status = ERROR;
		return (ERROR);
	}
	ret = execute_builtin(data, cmd);
	restore_std_fds(in_fd, out_fd);
	data->status = ret;
	return (ret);
}
