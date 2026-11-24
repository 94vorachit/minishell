/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   syntax.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/27 21:20:00 by vorhansa          #+#    #+#             */
/*   Updated: 2026/11/20 16:29:04 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/** Checks whether the input starts with a pipe.
 * 
 * If the token list is empty, it is valid.
 * If the first token is T_PIPE, it prints an error and returns SYNTAX_ERROR.
 * 
 * This prevents invalid commands like:
 * * `| ls`
 * * `|| ls` 
 */
int	ft_check_start_pipe(t_token *tokens)
{
	if (!tokens)
		return (SUCCESS);
	if (tokens->type == T_PIPE)
	{
		if (tokens->next && tokens->next->type == T_PIPE)
			ft_print_syntax_error_str("||");
		else
			ft_print_syntax_error(tokens);
		return (SYNTAX_ERROR);
	}
	return (SUCCESS);
}

/** Checks whether a pipe is in a bad position.
 * 
 * It triggers when:
 * * current token is T_PIPE
 * * and the next token is missing
 * * or the next token is also a pipe
 * 
 * This catches invalid patterns like:
 * * `ls |`
 * * `ls || cat` 
 */
int	ft_check_pipe_case(t_token *prev, t_token *cur, t_token *next)
{
	if (cur->type == T_PIPE && (!next || next->type == T_PIPE))
	{
		(void)prev;
		if (!next)
			ft_print_syntax_eof();
		else
			ft_print_syntax_error_str("||");
		return (SYNTAX_ERROR);
	}
	return (SUCCESS);
}

/** Checks whether a redirection token is followed by a valid word.
 * 
 * It triggers when:
 * * current token is a redirection
 * * and the next token is missing or not a word
 * 
 * This catches invalid patterns like:
 * * `> file`
 * * `cat <`
 * * `ls >>`
 */
int	ft_check_redir_case(t_token *cur, t_token *next)
{
	if (ft_isredir(cur->type) && (!next || next->type != T_WORD))
	{
		ft_print_syntax_error(next);
		return (SYNTAX_ERROR);
	}
	return (SUCCESS);
}

/** Main syntax validation function.
 * 1. checks if the command starts with a pipe
 * 2. walks through the token list
 * 3. checks each pipe case
 * 4. checks each redirection case
 * 5. returns `SUCCESS` if everything is valid, otherwise `SYNTAX_ERROR`
 * 
 * This is the main gate that decides if the parsed input is acceptable 
 * before execution. 
 */
int	ft_validate_syntax(t_token *tokens)
{
	t_token	*prev;
	t_token	*cur;
	t_token	*next;
	int		result;

	result = ft_check_start_pipe(tokens);
	if (result != SUCCESS)
		return (result);
	prev = NULL;
	cur = tokens;
	while (cur)
	{
		next = cur->next;
		result = ft_check_pipe_case(prev, cur, next);
		if (result != SUCCESS)
			return (result);
		result = ft_check_redir_case(cur, next);
		if (result != SUCCESS)
			return (result);
		prev = cur;
		cur = next;
	}
	return (SUCCESS);
}
