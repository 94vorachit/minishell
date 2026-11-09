/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand2.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/04 13:46:00 by vorhansa          #+#    #+#             */
/*   Updated: 2026/11/09 18:55:06 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/**
 * * Expands one token only.
 * * Uses expand_word(data, token->value).
 * * If expanded string is empty and the original word had no quotes, 
 * it removes the token from the list.
 * 
 * Why it matters:
 * 1. this handles shell behavior like:
 * * $EMPTY becomes empty
 * * but quoted empty strings must stay
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
 * Calls `expand_current_word` for each token.
 * Keeps track of previous and current token pointers.
 * If a token becomes empty and unquoted, removes it from the list.
 * 
 * How it works:
 * 1. start at first token
 * 2. for each token:
 * * if token is `T_WORD`, expand it
 * * if result is empty and no quotes were in original word, delete it
 * * otherwise keep it and continue
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
