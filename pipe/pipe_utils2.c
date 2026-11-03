/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_utils2.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/03 14:32:23 by vorhansa          #+#    #+#             */
/*   Updated: 2026/11/03 14:47:40 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	count_commands(t_cmd *cmd)
{
	int	count;

	count = 0;
	while (cmd)
	{
		count++;
		cmd = cmd->next;
	}
	return (count);
}

void	setup_child_io(t_cmd *cur, int prev_fd, int pipefd[2])
{
	if (prev_fd != -1)
	{
		dup2(prev_fd, STDIN_FILENO);
		close(prev_fd);
	}
	if (cur->next)
	{
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[0]);
		close(pipefd[1]);
	}
}

int	redirect_child_cmd(t_cmd *cur)
{
	if (cur->infile && redirect_input(cur) == -1)
		return (-1);
	if (cur->outfile && redirect_output(cur) == -1)
		return (-1);
	return (0);
}

void	update_parent_pipe(int *prev_fd, t_cmd *cur, int pipefd[2])
{
	if (*prev_fd != -1)
		close(*prev_fd);
	if (cur->next)
	{
		close(pipefd[1]);
		*prev_fd = pipefd[0];
	}
}

void	run_child_cmd(t_data *data, t_cmd *cur, int prev_fd, int pipefd[2])
{
	char	*path;
	char	**envp;

	setup_child_io(cur, prev_fd, pipefd);
	if (redirect_child_cmd(cur) == -1)
		exit(1);
	if (is_builtin(cur->argv[0]))
		exit(execute_builtin(data, cur));
	path = find_executable_path(data->env, cur->argv[0]);
	envp = env_to_char_array(data->env);
	if (!path || !envp)
		exit(127);
	execve(path, cur->argv, envp);
	perror("execve");
	exit(127);
}
