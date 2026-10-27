/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   syntax.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/27 21:20:00 by vorhansa          #+#    #+#             */
/*   Updated: 2026/10/27 21:37:20 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	ft_isredirection(t_token_type type)
{
	return (type == T_REDIR_IN || type == T_REDIR_OUT
		|| type == T_APPEND || type == T_HEREDOC);
}

void	ft_print_syntax_error(t_token *token)
{
	ft_putstr_fd("minishell: syntax error near unexpected token `", STDERR);
	if (!token)
		ft_putstr_fd("newline", STDERR);
	else
		ft_putstr_fd(token->value, STDERR);
	ft_putstr_fd("'\n", STDERR);
}

int	ft_validate_syntax(t_token *tokens)
{
	t_token	*current;
	t_token	*next;

	if (!tokens)
		return (SUCCESS);
	if (tokens->type == T_PIPE)
	{
		ft_print_syntax_error(tokens);
		return (SYNTAX_ERROR);
	}
	current = tokens;
	while (current)
	{
		next = current->next;
		if (current->type == T_PIPE)
		{
			if (!next || next->type == T_PIPE)
			{
				ft_print_syntax_error(next);
				return (SYNTAX_ERROR);
			}
		}
		else if (ft_isredirection(current->type)
			&& (!next || next->type != T_WORD))
		{
			ft_print_syntax_error(next);
			return (SYNTAX_ERROR);
		}
		current = next;
	}
	return (SUCCESS);
}
