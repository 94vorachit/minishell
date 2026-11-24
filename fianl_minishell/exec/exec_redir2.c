/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_redir2.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/05 20:33:48 by vorhansa          #+#    #+#             */
/*   Updated: 2026/11/20 16:29:04 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/**
 * Purpose: prepare output target file for redirection.
 * 
 * How it works:
 * 
 * 1. opens file with `O_CREAT | O_TRUNC` for `>`
 * 2. or `O_APPEND` for `>>`
 * 3. prints error if file cannot be opened
 */
int	ft_open_output_redir(char *value, t_token_type type)
{
	int	fd;

	if (type == T_APPEND)
		fd = open(value, O_WRONLY | O_CREAT | O_APPEND, 0644);
	else
		fd = open(value, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd < 0)
	{
		if (errno == ENOENT)
		{
			ft_putstr_fd("minishell: no such file or directory: ", STDERR);
			ft_putstr_fd(value, STDERR);
			ft_putstr_fd("\n", STDERR);
		}
		else
			perror(value);
		free(value);
		return (ERROR);
	}
	close(fd);
	return (0);
}

/**
 * Purpose: assign a redirection token to `t_cmd`.
 * 
 * How it works:
 * 1. ensure next token is a word
 * 2. duplicate filename
 * 3. if output redirect: validate file opening
 * 4. then assign into `cmd`
 */
int	ft_set_redir(t_cmd *cmd, t_token *token)
{
	char	*value;
	int		ret;

	if (!token->next || token->next->type != T_WORD)
		return (-1);
	value = ft_strdup(token->next->value);
	if (!value)
		return (-1);
	if (token->type == T_REDIR_OUT || token->type == T_APPEND)
	{
		ret = ft_open_output_redir(value, token->type);
		if (ret != 0)
			return (ret);
	}
	ft_assign_redir(cmd, token->type, value);
	return (0);
}

/**
 * Purpose: process `<` redirection.
 * 
 * How it works:
 * 1. duplicates the input file name
 * 2. if file does not exist, stores it in `cmd->infile`
 * 3. otherwise frees duplicate and returns success
 */
int	ft_handle_redir_in(t_cmd *cmd, t_token *cur)
{
	char	*value;

	value = ft_strdup(cur->next->value);
	if (!value)
		return (-1);
	if (access(value, F_OK) != 0)
	{
		free(cmd->infile);
		cmd->infile = value;
		return (1);
	}
	free(value);
	return (0);
}

/**
 * Purpose: validate and apply one redirection operator token.
 *
 * How it works:
 * 1. checks that the redirection has a following `T_WORD` target
 * 2. reports `ambiguous redirect` when target token is missing/invalid
 * 3. for non-pipeline commands, checks `< file` existence early
 * 4. on missing infile, prints `minishell: <file>: No such file or directory`
 * 5. delegates actual assignment/open checks to `ft_set_redir`
 *
 * Return:
 * * `SUCCESS` when the redirection is valid and applied
 * * `ERROR` when validation or setup fails
 */
int	ft_handle_redir_token(t_token *cur, int in_pipeline, t_cmd *cmd)
{
	if (!cur->next || cur->next->type != T_WORD)
	{
		ft_putstr_fd("minishell: ambiguous redirect\n", STDERR);
		return (ERROR);
	}
	if (!in_pipeline && cur->type == T_REDIR_IN
		&& access(cur->next->value, F_OK) != 0)
	{
		ft_putstr_fd("minishell: ", STDERR);
		ft_putstr_fd(cur->next->value, STDERR);
		ft_putstr_fd(": No such file or directory\n", STDERR);
		return (ERROR);
	}
	if (ft_set_redir(cmd, cur) != SUCCESS)
		return (ERROR);
	return (SUCCESS);
}

/**
 * Purpose: walk a command segment and collect all redirections into `t_cmd`.
 *
 * How it works:
 * 1. determines whether current token range belongs to a pipeline command
 * 2. iterates tokens until `T_PIPE` boundary
 * 3. when a redirection operator is found, calls `ft_handle_redir_token`
 * 4. skips the consumed redirection target token
 * 5. returns immediately on the first redirection error
 *
 * This function keeps token traversal logic centralized and delegates
 * redirection validation details to `ft_handle_redir_token`.
 */
int	ft_parse_redir(t_token *tokens, t_cmd *cmd)
{
	t_token	*cur;
	int		in_pipeline;

	cur = tokens;
	in_pipeline = ft_has_pipe(tokens);
	while (cur && cur->type != T_PIPE)
	{
		if (ft_isredir(cur->type))
		{
			if (ft_handle_redir_token(cur, in_pipeline, cmd) != SUCCESS)
				return (ERROR);
			cur = cur->next;
		}
		cur = cur->next;
	}
	return (SUCCESS);
}
