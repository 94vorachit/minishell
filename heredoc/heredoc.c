/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/05 14:18:47 by vorhansa          #+#    #+#             */
/*   Updated: 2026/11/09 19:58:40 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/**
 * Purpose: the child process that reads the heredoc text.
 * 
 * How it works:
 * 1. calls `sig_heredoc()` to configure signal behavior for heredoc mode
 * 2. closes the read-end of the pipe
 * 3. calls `write_heredoc_to_pipe(cmd->heredoc, pipefd[1])`
 * 4. closes the write-end
 * 5. exits with `0`
 * 
 * This runs in a child process so the shell can keep waiting without 
 * blocking the main shell loop.
 */
void	run_heredoc_child(t_cmd *cmd, int pipefd[2])
{
	sig_heredoc();
	close(pipefd[0]);
	write_heredoc_to_pipe(cmd->heredoc, pipefd[1]);
	close(pipefd[1]);
	exit(0);
}

/**
 * Purpose: wait for the heredoc child to finish and check whether 
 * it exited normally.
 * 
 * How it works:
 * 1. closes the write-end of the pipe
 * 2. calls `waitpid(pid, &status, 0)`
 * 3. if the child was interrupted by a signal, or exited with 
 * a non-zero code, it returns `-1`
 * 
 * This is a safety check:
 * 1. if heredoc creation failed, parent stops and reports failure
 * 2. if signal interrupted it, shell treats it as error
 */
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
 * Purpose: start the heredoc child and manage signal state around it.
 * 
 * How it works:
 * 1. calls `sig_ignore()` before forking
 * 2. does `fork()`
 * 3. if fork fails, closes both pipe ends and resets signals
 * 4. child executes `run_heredoc_child`
 * 5. parent waits with `wait_heredoc_child`
 * 6. after success, restores normal signal handling with `sig_init()`
 * 
 * This function is the setup stage for the heredoc subprocess.
 */
int	start_heredoc_child(t_cmd *cmd, int pipefd[2])
{
	pid_t	pid;

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
	return (0);
}

/**
 * Purpose: prepare a single command’s heredoc.
 * 
 * How it works:
 * 1. if `cmd->heredoc` is empty or null, do nothing
 * 2. create a pipe with `pipe(pipefd)`
 * 3. start the heredoc child
 * 4. if success, save the read-end of the pipe as `cmd->heredoc_fd`
 * 
 * This is the key function that makes heredoc usable by the command executor:
 * 1. the parent keeps the read end of the pipe
 * 2. later, when command runs, it does something like 
 * `dup2(cmd->heredoc_fd, STDIN_FILENO)`
 */
int	prepare_heredoc_cmd(t_cmd *cmd)
{
	int		pipefd[2];

	if (!cmd->heredoc)
		return (0);
	if (pipe(pipefd) < 0)
		return (-1);
	if (start_heredoc_child(cmd, pipefd) < 0)
		return (-1);
	cmd->heredoc_fd = pipefd[0];
	return (0);
}

/**
 * Purpose: prepare heredoc for every command in a command list.
 * 
 * How it works:
 * 1. loop through each command node
 * 2. call `prepare_heredoc_cmd(cur)`
 * 3. if any one fails, return `-1`
 * 
 * This processes all heredocs before command execution begins.
 */
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
