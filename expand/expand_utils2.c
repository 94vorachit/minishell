/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_utils2.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/28 18:59:04 by vorhansa          #+#    #+#             */
/*   Updated: 2026/10/28 19:08:45 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/**
 * Toggles the single-quote state.
 * 
 * It flips `in_single` and advances the index by one.
 * This means quotes are tracked while scanning the word.
 */
int	handle_single_quote(int *in_single, size_t *i)
{
	*in_single = !(*in_single);
	(*i)++;
	return (0);
}

/**
 * Same behavior as single quote, but for double quotes.
 * 
 * It flips `in_double`.
 */
int	handle_double_quote(int *in_double, size_t *i)
{
	*in_double = !(*in_double);
	(*i)++;
	return (0);
}

/**
 * Appends a normal character directly to the output string.
 * 
 * It is used for characters that are not special:
 * 1. plain letters
 * 2. digits
 * 3. spaces
 * 4. punctuation
 */
int	append_literal_char(char **result, const char *word, size_t *i)
{
	*result = append_char(*result, word[*i]);
	if (!*result)
		return (-1);
	(*i)++;
	return (0);
}

/**
 * Handles `$?`
 * It converts the shell status value to a string with `ft_itoa`
 * Appends it to the result
 * Increments the index by 2 so the `?` and the following character 
 * are skipped correctly
 * 
 * Example:
 * * input: `$?`
 * * output: last exit status, like `0` or `1`
 */
int	expand_status_var(t_data *data, char **result, size_t *i)
{
	char	*status;

	status = ft_itoa(data->status);
	if (!status)
		return (-1);
	*result = append_text(*result, status);
	free(status);
	if (!*result)
		return (-1);
	*i += 2;
	return (0);
}
