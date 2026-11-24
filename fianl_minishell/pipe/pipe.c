/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/03 14:45:58 by vorhansa          #+#    #+#             */
/*   Updated: 2026/11/20 15:43:15 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/**
 * Purpose: run one command node inside a pipeline child process.
 * 
 * How it works:
 * 1. restores default signal handling in child context
 * 2. connects stdin/stdout to the proper pipe ends
 * 3. applies command-level input/output redirections
 * 4. if command is a builtin, executes it and exits with builtin status
 * 5. otherwise delegates to external command runner
 * 
 * This function is the child-side entry point used after fork
 * for each stage of the pipeline.
 */
void	ft_run_child_cmd(t_data *data, t_cmd *cur, int prev_fd, int pipefd[2])
{
	ft_sig_default();
	ft_setup_child_io(cur, prev_fd, pipefd);
	if (ft_redir_child_cmd(cur) == -1)
		exit(ERROR);
	if (ft_is_builtin(cur->argv[0]))
		exit(ft_execute_builtin(data, cur));
	ft_run_external_child_cmd(data, cur);
}

/**
 * Creates the pipeline and forks all child processes
 * 
 * Behavior:
 * 1. initializes prev_fd = -1
 * 2. walks the linked list of commands with cur
 * 3. if the current command is not the last one, it calls pipe(pipefd)
 * 4. forks a child for this command
 * 5. in the child: calls ft_run_child_cmd(...)
 * 6. in the parent: updates the pipe state with ft_update_parent_pipe(...)
 * 7. continues until all commands are processed
 * 
 * This is the key function that wires the pipeline.
 */
int	ft_fork_pipe_cmds(t_data *data, t_cmd *cmd, pid_t *pids)
{
	int		pipefd[2];
	int		prev_fd;
	t_cmd	*cur;
	int		i;

	prev_fd = -1;
	cur = cmd;
	i = 0;
	while (cur)
	{
		if (cur->next && pipe(pipefd) < 0)
		{
			perror("pipe");
			return (-1);
		}
		pids[i] = fork();
		if (pids[i] == 0)
			ft_run_child_cmd(data, cur, prev_fd, pipefd);
		ft_update_parent_pipe(&prev_fd, cur, pipefd);
		cur = cur->next;
		i++;
	}
	return (0);
}

/**
 * Waits for every child in the pipeline to finish
 * 
 * Behavior:
 * 1. loops from i = 0 to count - 1
 * 2. calls waitpid(pids[i], &status, 0)
 * 3. converts each child’s termination status with ft_set_child_status(...)
 * 
 * This function is important because shell pipelines should wait for 
 * all stages before returning.
 */
void	ft_wait_pipel_child(t_data *data, pid_t *pids, int count)
{
	int	i;
	int	status;

	ft_sig_ignore();
	i = 0;
	while (i < count)
	{
		waitpid(pids[i], &status, 0);
		ft_set_child_status(data, status);
		i++;
	}
	ft_sig_init();
}

/**
 * Public entry point for executing a pipeline
 * 
 * Behavior:
 * 1. counts commands with ft_count_cmds(cmd)
 * 2. allocates a PID array
 * 3. calls ft_fork_pipe_cmds(...)
 * 4. if fork fails, frees the array and returns ERROR
 * 5. waits for the children
 * 6. frees the PID array
 * 7. returns data->status
 * 
 * This is the main pipeline runner used by the shell.
 */
int	ft_exec_pipe(t_data *data, t_cmd *cmd)
{
	pid_t	*pids;
	int		count;

	count = ft_count_cmds(cmd);
	pids = malloc(sizeof(pid_t) * count);
	if (!pids)
		return (ERROR);
	if (ft_prepare_heredoc_list(cmd) == -1)
	{
		free(pids);
		if (g_signal == SIGINT || g_signal == SIGQUIT)
			data->status = 128 + g_signal;
		else
			data->status = 130;
		return (data->status);
	}
	if (ft_fork_pipe_cmds(data, cmd, pids) == -1)
	{
		free(pids);
		return (ERROR);
	}
	ft_wait_pipel_child(data, pids, count);
	free(pids);
	return (data->status);
}

/**
 * void	ft_wait_pipel_child(t_data *data, pid_t *pids, int count)
{
	int	i;
	int	status;

	ft_sig_ignore();
	i = 0;
	while (i < count)
	{
		// waitpid(pids[i], &status, 0);
		while (waitpid(pids[i], &status, 0) == -1)
		{
			if (errno != EINTR)
			{
				perror("waitpid");
				data->status = 1;
				ft_sig_init();
				return ;
			}
		}
		ft_set_child_status(data, status);
		i++;
	}
	ft_sig_init();
}
 */
