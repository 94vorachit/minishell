/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/05 14:18:47 by vorhansa          #+#    #+#             */
/*   Updated: 2026/11/23 14:44:18 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/**
 * Purpose: run heredoc input collection inside a child process.
 *
 * How it works:
 * 1. configures heredoc signal behavior
 * 2. closes the pipe read-end
 * 3. when there is no argv, writes heredoc text into `outfile`
 *    (and optionally appends `infile` content after it)
 * 4. otherwise writes heredoc data to the pipe write-end
 *    (and optionally appends `infile` content after it)
 * 5. closes active write fd and exits
 *
 * This keeps heredoc reading separate from the main shell loop.
 */
void	ft_run_heredoc_child(t_cmd *cmd, int pipefd[2])
{
	int	output_fd;

	ft_sig_heredoc();
	close(pipefd[0]);
	if (!cmd->argv || !cmd->argv[0])
	{
		output_fd = -1;
		if (cmd->outfile)
			output_fd = ft_open_output_file(cmd);
		if (output_fd >= 0)
		{
			ft_write_heredoc_to_pipe(cmd->heredoc, output_fd);
			if (cmd->infile)
				ft_append_input_to_fd(cmd->infile, output_fd);
			close(output_fd);
		}
		close(pipefd[1]);
		exit(0);
	}
	ft_write_heredoc_to_pipe(cmd->heredoc, pipefd[1]);
	if (cmd->infile)
		ft_append_input_to_fd(cmd->infile, pipefd[1]);
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
int	ft_wait_heredoc_child(pid_t pid, int pipefd[2])
{
	int	status;

	close(pipefd[1]);
	waitpid(pid, &status, 0);
	if (WIFSIGNALED(status))
	{
		g_signal = WTERMSIG(status);
		close(pipefd[0]);
		return (-1);
	}
	if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
	{
		if (WEXITSTATUS(status) == 130)
			g_signal = SIGINT;
		close(pipefd[0]);
		return (-1);
	}
	return (0);
}

/**
 * Purpose: start the heredoc child and manage signal state around it.
 * 
 * How it works:
 * 1. calls `ft_sig_ignore()` before forking
 * 2. does `fork()`
 * 3. if fork fails, closes both pipe ends and resets signals
 * 4. child executes `ft_run_heredoc_child`
 * 5. parent waits with `ft_wait_heredoc_child`
 * 6. after success, restores normal signal handling with `ft_sig_init()`
 * 
 * This function is the setup stage for the heredoc subprocess.
 */
int	ft_start_heredoc_child(t_cmd *cmd, int pipefd[2])
{
	pid_t	pid;

	ft_sig_ignore();
	pid = fork();
	if (pid < 0)
	{
		close(pipefd[0]);
		close(pipefd[1]);
		ft_sig_init();
		return (-1);
	}
	if (pid == 0)
		ft_run_heredoc_child(cmd, pipefd);
	if (ft_wait_heredoc_child(pid, pipefd) < 0)
	{
		ft_sig_init();
		return (-1);
	}
	ft_sig_init();
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
int	ft_prepare_heredoc_cmd(t_cmd *cmd)
{
	int		pipefd[2];
	int		i;

	if (!cmd->heredocs)
		return (0);
	i = 0;
	while (cmd->heredocs[i])
	{
		free(cmd->heredoc);
		cmd->heredoc = ft_strdup(cmd->heredocs[i]);
		if (!cmd->heredoc)
			return (-1);
		if (pipe(pipefd) < 0)
			return (-1);
		if (ft_start_heredoc_child(cmd, pipefd) < 0)
			return (-1);
		if (cmd->heredoc_fd != -1)
			close(cmd->heredoc_fd);
		if (!cmd->heredocs[i + 1])
			cmd->heredoc_fd = pipefd[0];
		else
			close(pipefd[0]);
		i++;
	}
	return (0);
}

/**
 * Purpose: prepare heredoc for every command in a command list.
 * 
 * How it works:
 * 1. loop through each command node
 * 2. call `ft_prepare_heredoc_cmd(cur)`
 * 3. if any one fails, return `-1`
 * 
 * This processes all heredocs before command execution begins.
 */
int	ft_prepare_heredoc_list(t_cmd *cmds)
{
	t_cmd	*cur;

	cur = cmds;
	while (cur)
	{
		if (ft_prepare_heredoc_cmd(cur) == -1)
			return (-1);
		cur = cur->next;
	}
	return (0);
}
