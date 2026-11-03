/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/03 14:45:58 by vorhansa          #+#    #+#             */
/*   Updated: 2026/11/03 18:03:15 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/**
 * Creates the pipeline and forks all child processes
 * 
 * Behavior:
 * 1. initializes prev_fd = -1
 * 2. walks the linked list of commands with cur
 * 3. if the current command is not the last one, it calls pipe(pipefd)
 * 4. forks a child for this command
 * 5. in the child: calls run_child_cmd(...)
 * 6. in the parent: updates the pipe state with update_parent_pipe(...)
 * 7. continues until all commands are processed
 * 
 * This is the key function that wires the pipeline.
 */
int	fork_pipeline_cmds(t_data *data, t_cmd *cmd, pid_t *pids)
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
			run_child_cmd(data, cur, prev_fd, pipefd);
		update_parent_pipe(&prev_fd, cur, pipefd);
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
 * 3. converts each child’s termination status with set_child_status(...)
 * 
 * This function is important because shell pipelines should wait for 
 * all stages before returning.
 */
void	wait_pipeline_children(t_data *data, pid_t *pids, int count)
{
	int	i;
	int	status;

	sig_ignore();
	i = 0;
	while (i < count)
	{
		waitpid(pids[i], &status, 0);
		set_child_status(data, status);
		i++;
	}
	sig_init();
}

/**
 * Public entry point for executing a pipeline
 * 
 * Behavior:
 * 1. counts commands with count_commands(cmd)
 * 2. allocates a PID array
 * 3. calls fork_pipeline_cmds(...)
 * 4. if fork fails, frees the array and returns ERROR
 * 5. waits for the children
 * 6. frees the PID array
 * 7. returns data->status
 * 
 * This is the main pipeline runner used by the shell.
 */
int	execute_pipeline(t_data *data, t_cmd *cmd)
{
	pid_t	*pids;
	int		count;

	count = count_commands(cmd);
	pids = malloc(sizeof(pid_t) * count);
	if (!pids)
		return (ERROR);
	if (fork_pipeline_cmds(data, cmd, pids) == -1)
	{
		free(pids);
		return (ERROR);
	}
	wait_pipeline_children(data, pids, count);
	free(pids);
	return (data->status);
}
