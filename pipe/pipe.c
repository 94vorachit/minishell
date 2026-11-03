/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/03 14:45:58 by vorhansa          #+#    #+#             */
/*   Updated: 2026/11/03 14:47:16 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

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

void	wait_pipeline_children(t_data *data, pid_t *pids, int count)
{
	int	i;
	int	status;

	i = 0;
	while (i < count)
	{
		waitpid(pids[i], &status, 0);
		set_child_status(data, status);
		i++;
	}
}

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
