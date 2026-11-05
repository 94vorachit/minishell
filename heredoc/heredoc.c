/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/05 14:18:47 by vorhansa          #+#    #+#             */
/*   Updated: 2026/11/05 17:21:10 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	write_heredoc_to_pipe(char *delim, int fd)
{
	char	*line;

	while (1)
	{
		line = readline("> ");
		if (!line)
			break ;
		if (ft_strcmp(line, delim) == 0)
		{
			free(line);
			break ;
		}
		write(fd, line, ft_strlen(line));
		write(fd, "\n", 1);
		free(line);
	}
	return (0);
}

void	run_heredoc_child(t_cmd *cmd, int pipefd[2])
{
	sig_heredoc();
	close(pipefd[0]);
	write_heredoc_to_pipe(cmd->heredoc, pipefd[1]);
	close(pipefd[1]);
	exit(0);
}

int	wait_heredoc_child(pid_t pid, int pipefd[2])
{
	int	status;

	close(pipefd[1]);
	waitpid(pid, &status, 0);
	if (WIFSIGNALED(status) || (WIFEXITED(status)
			&& WEXITSTATUS(status) != 0))
	{
		close(pipefd[0]);
		return (-1);
	}
	return (0);
}

/**
 * How this works:
 * 1.  a child reads heredoc lines until delimiter
 * 2. content is written into a pipe
 * 3. parent keeps the read-end as `cmd->heredoc_fd`
 * 4. later execution does `dup2(cmd->heredoc_fd, STDIN_FILENO)`
 */
int	prepare_heredoc_cmd(t_cmd *cmd)
{
	int		pipefd[2];
	pid_t	pid;

	if (!cmd->heredoc)
		return (0);
	if (pipe(pipefd) < 0)
		return (-1);
	sig_ignore();
	pid = fork();
	if (pid < 0)
	{
		close(pipefd[0]);
		close(pipefd[1]);
		sig_init();
		return (-1);
	}
	if (pid == 0)
		run_heredoc_child(cmd, pipefd);
	if (wait_heredoc_child(pid, pipefd) < 0)
	{
		sig_init();
		return (-1);
	}
	sig_init();
	cmd->heredoc_fd = pipefd[0];
	return (0);
}

int	prepare_heredoc_list(t_cmd *cmds)
{
	t_cmd	*cur;

	cur = cmds;
	while (cur)
	{
		if (prepare_heredoc_cmd(cur) == -1)
			return (-1);
		cur = cur->next;
	}
	return (0);
}
