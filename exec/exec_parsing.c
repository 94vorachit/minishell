/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_parsing.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/29 15:42:41 by vorhansa          #+#    #+#             */
/*   Updated: 2026/11/09 18:42:00 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/**
 * Purpose: count command arguments before a pipe.
 *
 * Needed to allocate the correct size for `cmd->argv`.
 *
 * How it works:
 * 1. scans tokens
 * 2. counts only `T_WORD`
 * 3. skips redirection operand token
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
 * Purpose: Fills `cmd->argv` with the actual command arguments.
 *
 * How it works:
 * 1. iterate tokens
 * 2. copy each word into array
 * 3. skip file names after redirections
 * 4. add `NULL` sentinel at end
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

/**
 * Purpose: initialize a command structure.
 * 
 * How it works:
 * 1. set `infile`, `outfile`, `heredoc`, and `argv` to `NULL`
 * 2. set fd values to `-1`
 * 3. reset flags
 */
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

/**
 * Purpose: handle parse result from redirection analysis.
 * 
 * How it works:
 * 1. if parse failed, free `argv`
 * 2. return error code
 */
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

/**
 * Purpose: construct a full command from tokens.
 * 
 * How it works:
 * 1. initialize command
 * 2. count args
 * 3. allocate `argv`
 * 4. fill arguments
 * 5. parse redirections
 * 6. finalize result
 */
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
