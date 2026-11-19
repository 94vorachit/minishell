/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   syntax2.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/19 18:50:07 by vorhansa          #+#    #+#             */
/*   Updated: 2026/11/19 18:53:24 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/**
 * Purpose: print a syntax error for token-based parser failures.
 *
 * How it works:
 * 1. prints the fixed prefix used for unexpected-token syntax errors
 * 2. prints `newline` when there is no token
 * 3. otherwise prints the token value itself
 * 4. closes with a quote and newline
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

/**
 * Purpose: print a syntax error when the unexpected token is already
 * available as a raw string.
 *
 * How it works:
 * 1. prints the same unexpected-token prefix as token-based errors
 * 2. prints the provided string directly
 * 3. closes with a quote and newline
 */
void	ft_print_syntax_error_str(char *value)
{
	ft_putstr_fd("minishell: syntax error near unexpected token `", STDERR);
	ft_putstr_fd(value, STDERR);
	ft_putstr_fd("'\n", STDERR);
}

/**
 * Purpose: print an end-of-file syntax error message.
 *
 * How it works:
 * 1. prints a fixed message indicating parser EOF was reached unexpectedly
 * 2. writes to STDERR so command output is unaffected
 */
void	ft_print_syntax_eof(void)
{
	ft_putstr_fd("minishell: syntax error: unexpected end of file\n", STDERR);
}
