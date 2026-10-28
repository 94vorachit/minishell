/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/28 18:57:50 by vorhansa          #+#    #+#             */
/*   Updated: 2026/10/28 19:08:31 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
 * Checks whether a character can start a variable name.
 
 * It accepts:
 * * letters: `a-z`, `A-Z`
 * * underscore: `_`
 * 
 * Example:
 * * valid: `USER`, `_tmp`
 * * invalid: `1abc`, `-abc`
 */
int	is_var_start(char c)
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
int	is_var_char(char c)
{
	return (ft_isalnum((unsigned char)c) || c == '_');
}

/**
 * Same as `is_var_start`.
 * It is used to parse only valid variable names when reading `$NAME`.
 * This keeps the variable name parsing strict and avoids accidentally 
 * consuming digits or punctuation.
*/
int	is_var_name_char(char c)
{
	return (ft_isalpha((unsigned char)c) || c == '_');
}

/**
 * Appends a string to the current result buffer.
 * 
 * It does:
 * 1. ignore empty input
 * 2. join old result + new text using `ft_strjoin`
 * 3. free the old result
 * 4. return the new combined string
 * 
 * This is the core helper for building the expanded string.
 */
char	*append_text(char *result, const char *text)
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
 * and delegates to `append_text`.
 */
char	*append_char(char *result, char c)
{
	char	part[2];

	part[0] = c;
	part[1] = '\0';
	return (append_text(result, part));
}
