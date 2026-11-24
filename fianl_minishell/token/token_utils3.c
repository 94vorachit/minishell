/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_utils3.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/19 19:43:26 by vorhansa          #+#    #+#             */
/*   Updated: 2026/11/20 16:29:04 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/**Checks whether a token type is a redirection.
 * 
 * It returns true for:
 * * `T_REDIR_IN`
 * * `T_REDIR_OUT`
 * * `T_APPEND`
 * * `T_HEREDOC`
 * 
 * This is used later to decide whether a token is a redirection operator.
 */
int	ft_isredir(t_token_type type)
{
	return (type == T_REDIR_IN || type == T_REDIR_OUT
		|| type == T_APPEND || type == T_HEREDOC);
}

/** Checks whether a character is one of the shell operators:
 * * `|`
 * * `<`
 * * `>`
 * 
 * This is used while scanning a word to stop reading when 
 * the parser hits an operator.
 */
int	ft_isoperator(char c)
{
	return (c == '|' || c == '<' || c == '>');
}
