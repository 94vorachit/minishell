/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand2.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/04 13:46:00 by vorhansa          #+#    #+#             */
/*   Updated: 2026/11/04 13:46:28 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/**
 * Expands one token.
 * Removes it if it becomes an unquoted empty string.
 * 
 * Returns:
 * * `-1`: expansion error
 * * `0`: token kept
 * * `1`: token removed
 */
int	expand_current_word(t_data *data, t_token **tokens,
				t_token **cur, t_token **pre)
{
	char	*exp;

	if ((*cur)->type != T_WORD)
		return (0);
	exp = expand_word(data, (*cur)->value);
	if (!exp)
		return (-1);
	if (exp[0] == '\0' && !word_has_quote((*cur)->value))
	{
		free(exp);
		remove_empty_word(tokens, cur, pre);
		return (1);
	}
	free((*cur)->value);
	(*cur)->value = exp;
	return (0);
}

/**
 * Walks through the entire token list.
 * Calls `expand_current_word`.
 * Maintains `current` and `previous` pointers.
 */
int	expand_tokens(t_data *data, t_token **tokens)
{
	t_token	*cur;
	t_token	*pre;
	int		result;

	cur = *tokens;
	pre = NULL;
	while (cur)
	{
		result = expand_current_word(data, tokens, &cur, &pre);
		if (result == -1)
			return (-1);
		if (result == 1)
			continue ;
		pre = cur;
		cur = cur->next;
	}
	return (0);
}
