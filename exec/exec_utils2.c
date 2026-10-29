/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_utils2.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/29 15:42:41 by vorhansa          #+#    #+#             */
/*   Updated: 2026/10/29 15:45:11 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/**
 * Counts how many `T_WORD` tokens appear before a pipe.
 * 
 * Needed to allocate the correct size for `cmd->argv`.
 * 
 * Behavior:
 * * Walks the token list until `T_PIPE`
 * * Increments `argc` for every word
 * * If the current token is a redirection, skips its operand token too
 */
int	count_args(t_token *tokens)
{
	t_token	*cur;
	int		argc;

	argc = 0;
	cur = tokens;
	while (cur && cur->type != T_PIPE)
	{
		if (cur->type == T_WORD)
			argc++;
		if (ft_isredirection(cur->type) && cur->next)
			cur = cur->next;
		cur = cur->next;
	}
	return (argc);
}

/**
 * Fills cmd->argv with the actual command arguments.
 * 
 * Behavior:
 * * Iterates tokens again
 * * For each T_WORD, duplicates the token value into cmd->argv[i]
 * * Adds a final NULL at the end
 * * If a redirection is found, skips the next token because that token 
 * is file name, not command arg
 */
int	fill_argv(t_token *tokens, t_cmd *cmd)
{
	t_token	*cur;
	int		i;

	i = 0;
	cur = tokens;
	while (cur && cur->type != T_PIPE)
	{
		if (cur->type == T_WORD)
		{
			cmd->argv[i] = ft_strdup(cur->value);
			if (!cmd->argv[i])
				return (-1);
			cmd->argv[++i] = NULL;
		}
		if (ft_isredirection(cur->type))
			cur = cur->next;
		cur = cur->next;
	}
	return (0);
}

/**
 * Replaces stdin with the file from `cmd->infile`.
 * 
 * Behavior:
 * 1. Opens the file with `open(..., O_RDONLY)`
 * 2. If fail: prints `perror(...)`
 * 3. Uses `dup2(fd, STDIN_FILENO)`
 * 4. Closes the file descriptor
 */
int	redirect_input(t_cmd *cmd)
{
	int	fd;

	fd = open(cmd->infile, O_RDONLY);
	if (fd < 0)
	{
		perror(cmd->infile);
		return (-1);
	}
	dup2(fd, STDIN_FILENO);
	close(fd);
	return (0);
}

/**
 * Replaces stdout with the output file.
 * 
 * Behavior:
 * 1. Opens the file with:
 * * `O_WRONLY | O_CREAT`
 * * `O_TRUNC` for `>`
 * * `O_APPEND` for `>>`
 * 2. Uses `dup2(fd, STDOUT_FILENO)`
 * 3. Closes the file
 */
int	redirect_output(t_cmd *cmd)
{
	int	fd;
	int	flags;

	flags = O_WRONLY | O_CREAT;
	if (cmd->append)
		flags |= O_APPEND;
	else
		flags |= O_TRUNC;
	fd = open(cmd->outfile, flags, 0644);
	if (fd < 0)
	{
		perror(cmd->outfile);
		return (-1);
	}
	dup2(fd, STDOUT_FILENO);
	close(fd);
	return (0);
}

/**
 * Builds one command structure from the token list.
 * 
 * Flow:
 * 1. Zero-initialize `cmd`
 * 2. Call `parse_redirections(tokens, cmd)` to store redirection files
 * 3. Count args with `count_args`
 * 4. Allocate `argv`
 * 5. Fill `argv` with `fill_argv`
 * 
 * This is the main parser for a single command.
 */
int	build_simple_cmd(t_token *tokens, t_cmd *cmd)
{
	int	argc;

	if (!cmd)
		return (-1);
	ft_bzero(cmd, sizeof(*cmd));
	if (parse_redirections(tokens, cmd) == -1)
	{
		ft_free_cmd(cmd);
		return (-1);
	}
	argc = count_args(tokens);
	cmd->argv = malloc(sizeof(char *) * (argc + 1));
	if (!cmd->argv)
	{
		ft_free_cmd(cmd);
		return (-1);
	}
	cmd->argv[0] = NULL;
	if (fill_argv(tokens, cmd) == -1)
	{
		ft_free_cmd(cmd);
		return (-1);
	}
	return (0);
}
