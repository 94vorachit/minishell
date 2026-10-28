/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   syntax.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/27 21:20:00 by vorhansa          #+#    #+#             */
/*   Updated: 2026/10/28 17:20:41 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	ft_print_syntax_error(t_token *token)
{
	ft_putstr_fd("minishell: syntax error near unexpected token `", STDERR);
	if (!token)
		ft_putstr_fd("newline", STDERR);
	else
		ft_putstr_fd(token->value, STDERR);
	ft_putstr_fd("'\n", STDERR);
}

static int	check_start_pipe(t_token *tokens)
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

static int	check_pipe_case(t_token *current, t_token *next)
{
	if (current->type == T_PIPE && (!next || next->type == T_PIPE))
	{
		ft_print_syntax_error(next);
		return (SYNTAX_ERROR);
	}
	return (SUCCESS);
}

static int	check_redirection_case(t_token *current, t_token *next)
{
	if (ft_isredirection(current->type) && (!next || next->type != T_WORD))
	{
		ft_print_syntax_error(next);
		return (SYNTAX_ERROR);
	}
	return (SUCCESS);
}

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
