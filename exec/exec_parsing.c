/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_parsing.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/29 15:42:41 by vorhansa          #+#    #+#             */
/*   Updated: 2026/11/09 16:26:45 by vorhansa         ###   ########.fr       */
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
			i++;
		}
		if (ft_isredirection(cur->type) && cur->next)
			cur = cur->next;
		cur = cur->next;
	}
	cmd->argv[i] = NULL;
	return (0);
}

int	init_simple_cmd(t_cmd *cmd)
{
	cmd->infile = NULL;
	cmd->outfile = NULL;
	cmd->heredoc = NULL;
	cmd->heredoc_fd = -1;
	cmd->append = 0;
	cmd->next = NULL;
	cmd->argv = NULL;
	return (0);
}

int	finalize_simple_cmd(t_cmd *cmd, int parse_ret)
{
	if (parse_ret == -1)
	{
		ft_free_split(cmd->argv);
		cmd->argv = NULL;
		return (-1);
	}
	if (parse_ret == ERROR)
	{
		ft_free_split(cmd->argv);
		cmd->argv = NULL;
		return (ERROR);
	}
	return (0);
}

int	build_simple_cmd(t_token *tokens, t_cmd *cmd)
{
	int	argc;
	int	parse_ret;

	init_simple_cmd(cmd);
	argc = count_args(tokens);
	cmd->argv = malloc(sizeof(char *) * (argc + 1));
	if (!cmd->argv)
		return (-1);
	if (fill_argv(tokens, cmd) == -1)
	{
		ft_free_split(cmd->argv);
		cmd->argv = NULL;
		return (-1);
	}
	parse_ret = parse_redirections(tokens, cmd);
	return (finalize_simple_cmd(cmd, parse_ret));
}
