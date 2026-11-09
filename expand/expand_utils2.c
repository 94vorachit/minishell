/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_utils2.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/28 18:59:04 by vorhansa          #+#    #+#             */
/*   Updated: 2026/11/09 19:35:02 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/**
 * Purpose: Toggles the single-quote state.
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
 * Purpose: Same behavior as single quote, but for double quotes.
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
 * Purpose: Appends a normal character directly to the output string.
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
 * 1. Handles `$?`
 * 2. Converts data->status to a string with ft_itoa
 * 3. Appends that value to output
 * 4. Moves index by 2 so both $ and ? are skipped
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

/**
 * 1. scans the original word
 * 2. returns `1` if it contains `'` or `"`
 * 3. otherwise returns `0`
 * 
 * Used to decide:
 * 1. if empty result should be removed
 * 2. or if empty quoted string should remain
 */
int	word_has_quote(const char *word)
{
	while (*word)
	{
		if (*word == '\'' || *word == '"')
			return (1);
		word++;
	}
	return (0);
}
