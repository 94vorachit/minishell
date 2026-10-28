/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   syntax.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/27 21:20:00 by vorhansa          #+#    #+#             */
/*   Updated: 2026/10/28 18:30:28 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/** Prints a shell-style syntax error message.
 * 
 * then either:
 * * the token value
 * * or newline if there is no next token
 * then a closing quote and newline
 * 
 * This function is used whenever the parser finds invalid token ordering.
*/
void	ft_print_syntax_error(t_token *token)
{
	ft_putstr_fd("minishell: syntax error near unexpected token `", STDERR);
	if (!token)
		ft_putstr_fd("newline", STDERR);
	else
		ft_putstr_fd(token->value, STDERR);
	ft_putstr_fd("'\n", STDERR);
}

/** Checks whether the input starts with a pipe.
 * 
 * If the token list is empty, it is valid.
 * If the first token is T_PIPE, it prints an error and returns SYNTAX_ERROR.
 * 
 * This prevents invalid commands like:
 * * `| ls`
 * * `|| ls` 
 */
int	check_start_pipe(t_token *tokens)
{
	if (!tokens)
		return (SUCCESS);
	if (tokens->type == T_PIPE)
	{
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
int	check_pipe_case(t_token *current, t_token *next)
{
	if (current->type == T_PIPE && (!next || next->type == T_PIPE))
	{
		ft_print_syntax_error(next);
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
int	check_redirection_case(t_token *current, t_token *next)
{
	if (ft_isredirection(current->type) && (!next || next->type != T_WORD))
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
	t_token	*current;
	t_token	*next;
	int		result;

	result = check_start_pipe(tokens);
	if (result != SUCCESS)
		return (result);
	current = tokens;
	while (current)
	{
		next = current->next;
		result = check_pipe_case(current, next);
		if (result != SUCCESS)
			return (result);
		result = check_redirection_case(current, next);
		if (result != SUCCESS)
			return (result);
		current = next;
	}
	return (SUCCESS);
}
