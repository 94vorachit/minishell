/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_redir3.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/09 16:24:05 by vorhansa          #+#    #+#             */
/*   Updated: 2026/11/09 17:48:59 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/**
 * Purpose: replace stdin with an input file.
 *
 * How it works:
 * 1. open file read-only
 * 2. `dup2(fd, STDIN_FILENO)`
 * 3. close old fd
 */
int	redirect_file(char *infile)
{
	int	fd;

	fd = open(infile, O_RDONLY);
	if (fd < 0)
	{
		if (errno == ENOENT)
		{
			ft_putstr_fd("minishell: no such file or directory: ", STDERR);
			ft_putstr_fd(infile, STDERR);
			ft_putstr_fd("\n", STDERR);
		}
		else
			perror(infile);
		return (-1);
	}
	if (dup2(fd, STDIN_FILENO) < 0)
	{
		perror("dup2");
		close(fd);
		return (-1);
	}
	close(fd);
	return (0);
}

/**
 * Purpose: apply input redirection to the current process.
 * 
 * How it works:
 * 1. if heredoc exists: use heredoc fd
 * 2. else use `redirect_file(cmd->infile)`
 */
int	redirect_input(t_cmd *cmd)
{
	if (cmd->heredoc_fd != -1)
	{
		if (dup2(cmd->heredoc_fd, STDIN_FILENO) < 0)
		{
			perror("dup2");
			return (-1);
		}
		close(cmd->heredoc_fd);
		cmd->heredoc_fd = -1;
		return (0);
	}
	return (redirect_file(cmd->infile));
}

/**
 * Purpose: open outfile for writing.
 * 
 * How it works:
 * 1. append mode uses `O_APPEND`
 * 2. normal output uses `O_TRUNC`
 */
int	open_output_file(t_cmd *cmd)
{
	if (cmd->append)
		return (open(cmd->outfile, O_WRONLY | O_CREAT | O_APPEND, 0644));
	return (open(cmd->outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644));
}

/**
 * Purpose: replace stdout with output file.
 * 
 * How it works:
 * 1. open file
 * 2. `dup2(fd, STDOUT_FILENO)`
 * 3. close fd
 */
int	redirect_output(t_cmd *cmd)
{
	int	fd;

	if (!cmd->outfile)
		return (0);
	fd = open_output_file(cmd);
	if (fd < 0)
	{
		if (errno == ENOENT)
		{
			ft_putstr_fd("minishell: no such file or directory: ", STDERR);
			ft_putstr_fd(cmd->outfile, STDERR);
			ft_putstr_fd("\n", STDERR);
		}
		else
			perror(cmd->outfile);
		return (-1);
	}
	if (dup2(fd, STDOUT_FILENO) < 0)
	{
		perror("dup2");
		close(fd);
		return (-1);
	}
	close(fd);
	return (0);
}
