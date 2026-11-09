/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_redir2.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/05 20:33:48 by vorhansa          #+#    #+#             */
/*   Updated: 2026/11/09 17:45:19 by vorhansa         ###   ########.fr       */
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
int	open_output_redir(char *value, t_token_type type)
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
int	set_redirection(t_cmd *cmd, t_token *token)
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
		ret = open_output_redir(value, token->type);
		if (ret != 0)
			return (ret);
	}
	assign_redirection(cmd, token->type, value);
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
int	handle_redir_in(t_cmd *cmd, t_token *cur)
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
 * Purpose: scan tokens and extract all redirections in the command.
 * 
 * How it works:
 * 1. walk token list until pipe
 * 2. detect redirection token
 * 3. validate next token
 * 4. call `set_redirection` or `handle_redir_in`
 * 5. continue scanning
 */
int	parse_redirections(t_token *tokens, t_cmd *cmd)
{
	t_token	*cur;

	cur = tokens;
	while (cur && cur->type != T_PIPE)
	{
		if (ft_isredirection(cur->type))
		{
			if (!cur->next || cur->next->type != T_WORD)
				return (-1);
			if ((cur->type == T_REDIR_IN)
				&& (handle_redir_in(cmd, cur) != SUCCESS))
			{
				if ((handle_redir_in(cmd, cur) < 0))
					return (-1);
				else
					return (0);
			}
			if (set_redirection(cmd, cur) != SUCCESS)
				return (ERROR);
			cur = cur->next;
		}
		cur = cur->next;
	}
	return (SUCCESS);
}

/*int parse_redirections(t_token *tokens, t_cmd *cmd)
{
	t_token *cur;
	int     ret;

	cur = tokens;
	while (cur && cur->type != T_PIPE)
	{
		if (ft_isredirection(cur->type))
		{
			if (!cur->next || cur->next->type != T_WORD)
				return (-1);
			if (cur->type == T_REDIR_IN)
			{
				ret = handle_redir_in(cmd, cur);
				if (ret < 0)
					return (-1);
				if (ret > 0)
					return (0);
			}
			ret = set_redirection(cmd, cur);
			if (ret != 0)
				return (ret);
			cur = cur->next;
		}
		cur = cur->next;
	}
	return (SUCCESS);
}*/