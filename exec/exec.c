/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/28 19:36:16 by vorhansa          #+#    #+#             */
/*   Updated: 2026/11/10 15:27:44 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/**
 * Purpose: actual child process logic for a command.
 * 
 * How it works:
 * 1. if there is input redirection, call `redirect_input`
 * 2. if there is output redirection, call `redirect_output`
 * 3. then `execve` runs the real program
 * 4. on failure, exits with 126
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
 * Purpose: convert raw wait status into shell status.
 * 
 * How it works:
 * 1. if process exited normally: use `WEXITSTATUS`
 * 2. if terminated by signal:
 * * `SIGINT` -> 130
 * * `SIGQUIT` -> 131
 * * others -> `128 + signal`
 * 3. stores result in `data->status`
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
 * Purpose: generic process runner used for external command execution flow.
 * 
 * How it works:
 * 1. `fork()`
 * 2. child runs `run_child`
 * 3. parent waits
 * 4. frees memory
 * 5. stores shell status via `set_child_status`
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