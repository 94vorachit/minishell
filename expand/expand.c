/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/28 15:05:12 by vorhansa          #+#    #+#             */
/*   Updated: 2026/11/09 19:32:39 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/**
 * Purpose: Handles `$VAR`
 *
 * 1. Reads the variable name after `$`
 * 2. Example: `$USER`
 * 3. Extracts `"USER"`
 * 4. Calls `env_get(data->env, name)`
 * 5. If variable exists, appends its value to result
 * 
 * How it works:
 * 1. find start after `$`
 * 2. read characters while valid in a variable name
 * 3. `ft_substr` to get the name
 * 4. look up in environment
 * 5. append value if found
 * 
 * If the variable is undefined, it does nothing and leaves the result 
 * unchanged.
 * 
 * Example:
 * input: `$USER`
 * env: `USER=alice`
 * output: `alice`
 */
int	expand_env_var(t_data *data, const char *word, size_t *i, char **result)
{
	size_t	start;
	char	*name;
	char	*value;

	start = *i + 1;
	*i = start;
	while (word[*i] && is_var_name_char(word[*i]))
		(*i)++;
	name = ft_substr(word, start, *i - start);
	if (!name)
		return (-1);
	value = env_get(data->env, name);
	free(name);
	if (value)
	{
		*result = append_text(*result, value);
		if (!*result)
			return (-1);
	}
	return (0);
}

/**
 * Purpose: This is the main decision function when a $ is found.
 * 
 * Cases:
 * 1. `$` at the end of the string: keep $ literally
 * 2. `$?`: expand exit status
 * 3. `$NAME`: expand environment variable
 * 4. otherwise: keep `$` as a literal char and continue
 * 
 * Example:
 * 1. input: `$USER` -> `alice`
 * 2. input: `$?` -> `0` or `1`
 * 3. input: `$` -> `$`
 * 
 * This function decides what to do when the parser sees `$`.
 */
int	handle_dollar(t_data *data, const char *word, size_t *i, char **result)
{
	if (word[*i + 1] == '\0')
	{
		*result = append_char(*result, '$');
		if (!*result)
			return (-1);
		(*i)++;
		return (0);
	}
	if (word[*i + 1] == '?')
		return (expand_status_var(data, result, i));
	if (is_var_start(word[*i + 1]))
		return (expand_env_var(data, word, i, result));
	*result = append_char(*result, '$');
	if (!*result)
		return (-1);
	(*i)++;
	return (0);
}

/**
 * Purpose: Main state-based character processor.
 * 
 * It checks each character and does:
 * 1. if it is `'` and not inside double quotes: toggle single quote state
 * 2. if it is `"` and not inside single quotes: toggle double quote state
 * 3. if it is `$` and not inside single quotes: call `handle_dollar`
 * 4. otherwise: append the character literally
 * 
 * This is how the shell knows:
 * * inside single quotes: no variable expansion
 * * inside double quotes: variable expansion still happens
 */
int	process_char(t_exp_state *state)
{
	if (state->word[*(state->i)] == '\'' && !(state->in_double))
		return (handle_single_quote(&state->in_single, state->i));
	if (state->word[*(state->i)] == '"' && !(state->in_single))
		return (handle_double_quote(&state->in_double, state->i));
	if (state->word[*(state->i)] == '$' && !(state->in_single))
	{
		if (handle_dollar(state->data, state->word, state->i, state->result)
			!= 0)
			return (-1);
		return (0);
	}
	return (append_literal_char(state->result, state->word, state->i));
}

/**
 * Purpose: Expands the variable references in one word.
 *
 * 1. Expands the contents of one word string.
 * 2. Loops character by character.
 * 3. Uses a state machine with:
 * * `in_single`
 * * `in_double`
 * * current index `i`
 * * result buffer
 * 
 * How it works:
 * 1. start with empty string
 * 2. process each character
 * 3. if char is `$` and not inside single quotes, call `handle_dollar`
 * 4. otherwise append the char literally
 * 5. at the end, return expanded final string
 * 
 * This function is responsible for turning:
 * * `hello$USER`
 * * `$HOME/bin`
 * * `"$USER"`
 * 
 * into their expanded versions.
 */
char	*expand_word(t_data *data, const char *word)
{
	char		*result;
	size_t		i;
	t_exp_state	state;

	if (!data || !word)
		return (NULL);
	result = ft_strdup("");
	if (!result)
		return (NULL);
	i = 0;
	state.data = data;
	state.word = word;
	state.i = &i;
	state.result = &result;
	state.in_single = 0;
	state.in_double = 0;
	while (word[i])
	{
		if (process_char(&state) != 0)
			return (free(result), NULL);
	}
	return (result);
}

/**
 * Purpose: removes a token safely
 * 
 * 1. Saves the token after the current one.
 * 2. Links the previous token directly to that next token.
 * 3. Updates the list head if removing the first token.
 * 4. Frees the token value and token structure.
 * 5. Updates `current` to the next token.
 */
void	remove_empty_word(t_token **tokens, t_token **cur, t_token **pre)
{
	t_token	*next;

	next = (*cur)->next;
	if (*pre)
		(*pre)->next = next;
	else
		*tokens = next;
	free((*cur)->value);
	free(*cur);
	*cur = next;
}
