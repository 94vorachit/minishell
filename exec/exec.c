/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/28 19:36:16 by vorhansa          #+#    #+#             */
/*   Updated: 2026/11/09 17:00:30 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/**
 * Runs the actual command in the child process.
 * 
 * Behavior:
 * 1. If `cmd->infile` exists, call `redirect_input`
 * 2. If `cmd->outfile` exists, call `redirect_output`
 * 3. Then run:
 * `execve(exec_path, cmd->argv, envp)`
 * 4. If `execve` fails, print `perror("execve")` and exit with `127`
 * 
 * This is where the process actually becomes the target command.
 */
void	run_child(t_cmd *cmd, char *exec_path, char **envp)
{
	if ((cmd->infile || cmd->heredoc_fd != -1) && redirect_input(cmd) == -1)
	{
		free(exec_path);
		ft_free_split(envp);
		exit(1);
	}
	if (cmd->outfile && redirect_output(cmd) == -1)
	{
		free(exec_path);
		ft_free_split(envp);
		exit(1);
	}
	execve(exec_path, cmd->argv, envp);
	perror(exec_path);
	free(exec_path);
	ft_free_split(envp);
	exit(126);
}

/**
 * Converts the child exit status into the shell status.
 * 
 * The exit status is converted from `waitpid` using the normal shell rule:
 * - `Ctrl+C` -> 130
 * - `Ctrl+\` -> 131
 *
 * Behavior:
 * * If child exited normally:
 * `data->status = WEXITSTATUS(status)`
 * * Else if it was terminated by a signal:
 * `data->status = 128 + WTERMSIG(status)`
 */
void	set_child_status(t_data *data, int status)
{
	if (WIFEXITED(status))
	{
		data->status = WEXITSTATUS(status);
		g_signal = 0;
		return ;
	}
	if (WIFSIGNALED(status))
	{
		if (WTERMSIG(status) == SIGINT)
			data->status = 130;
		else if (WTERMSIG(status) == SIGQUIT)
			data->status = 131;
		else
			data->status = 128 + WTERMSIG(status);
		g_signal = 0;
		return ;
	}
	data->status = 1;
	g_signal = 0;
}

/**
 * Forks and waits for the command.
 * 
 * Flow:
 * 1. `fork()`
 * 2. Child: `run_child(...)`
 * 3. Parent: `waitpid(pid, &status, 0)`
 * 4. Free memory
 * 5. Call `set_child_status(...)`
 * 
 * This is the execution wrapper around the child.
 */
int	run_process(t_data *data, t_cmd *cmd, char *exec_path, char **envp)
{
	pid_t	pid;
	int		status;

	pid = fork();
	if (pid < 0)
	{
		perror("fork");
		free(exec_path);
		ft_free_split(envp);
		data->status = ERROR;
		return (ERROR);
	}
	if (pid == 0)
		run_child(cmd, exec_path, envp);
	waitpid(pid, &status, 0);
	free(exec_path);
	ft_free_split(envp);
	set_child_status(data, status);
	return (data->status);
}

/*
int	run_process(t_data *data, t_cmd *cmd, char *exec_path, char **envp)
{
	pid_t	pid;
	int		status;

	pid = fork();
	if (pid < 0)
	{
		perror("fork");
		free(exec_path);
		ft_free_split(envp);
		data->status = ERROR;
		return (ERROR);
	}
	if (pid == 0)
		run_child(cmd, exec_path, envp);
	// waitpid(pid, &status, 0);
	while (waitpid(pid, &status, 0) == -1)
	{
		if (errno != EINTR)
		{
			perror("waitpid");
			free(exec_path);
			ft_free_split(envp);
			data->status = 1;
			return (ERROR);
		}
	}
	free(exec_path);
	ft_free_split(envp);
	set_child_status(data, status);
	return (data->status);
}*/