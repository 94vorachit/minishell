/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/28 18:57:50 by vorhansa          #+#    #+#             */
/*   Updated: 2026/11/23 15:39:48 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
 * Purpose: Checks whether a character can start a variable name.
 *
 * It accepts:
 * * letters: `a-z`, `A-Z`
 * * underscore: `_`
 * 
 * Example:
 * * valid: `USER`, `_tmp`
 * * invalid: `1abc`, `-abc`
 */
int	ft_is_var_start(char c)
{
	return (ft_isalpha((unsigned char)c) || c == '_');
}

/**
 * Checks whether a character is allowed inside a variable name after 
 * the first char.
 * 
 * Accepts:
 * * letters `a-z`, `A-Z`
 * * digits `0-9`
 * * underscore `_`
 * 
 * Example:
 * * valid: `USER123`
 * * invalid: `HOME-123`
*/
int	ft_is_var_char(char c)
{
	return (ft_isalnum((unsigned char)c) || c == '_');
}

/**
 * Purpose: Appends a string to the current result buffer.
 * 
 * It does:
 * 1. ignore empty input
 * 2. join old result + new text using `ft_strjoin`
 * 3. free the old result
 * 4. return the new combined string
 * 
 * This is the core helper for building the expanded string.
 */
char	*ft_append_text(char *result, const char *text)
{
	char	*joined;

	if (!text || text[0] == '\0')
		return (result);
	joined = ft_strjoin(result, text);
	if (!joined)
	{
		free(result);
		return (NULL);
	}
	free(result);
	return (joined);
}

/**
 * Appends a single character to the result.
 * It creates a 2-byte local string (`char part[2]`) 
 * and delegates to `ft_append_text`.
 */
char	*ft_append_char(char *result, char c)
{
	char	part[2];

	part[0] = c;
	part[1] = '\0';
	return (ft_append_text(result, part));
}

/**
 * prevent the shell from interpreting $$
 * 
 * if (word[*i + 1] == '$')
 *		return (ft_handle_double_dollar(result, i));
 *	at line 94 in handle dollar
 */
int	ft_handle_double_dollar(char **result, size_t *i)
{
	*result = ft_append_char(*result, '$');
	if (!*result)
		return (-1);
	*result = ft_append_char(*result, '$');
	if (!*result)
		return (-1);
	*i += 2;
	return (0);
}
