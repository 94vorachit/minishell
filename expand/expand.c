/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/28 15:05:12 by vorhansa          #+#    #+#             */
/*   Updated: 2026/10/28 19:09:47 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/**
 * Handles `$VAR`
 *
 * 1. starts at the char after `$`
 * 2. reads the variable name
 * 3. extracts the name with `ft_substr`
 * 4. looks it up in the environment using `env_get`
 * 5. appends the value if found
 * 
 * If the variable is undefined, it does nothing and leaves the result unchanged.
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
 * Central logic for $ processing.
 * 
 * Cases:
 * 1. `$` at the end of the string: keep $ literally
 * 2. `$?`: expand exit status
 * 3. `$NAME`: expand environment variable
 * 4. otherwise: keep `$` as a literal char and continue
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
 * Main state-based character processor.
 * 
 * It looks at one character and decides what to do:
 * 1. if it is `'` and not inside double quotes: toggle single quote state
 * 2. if it is `"` and not inside single quotes: toggle double quote state
 * 3. if it is `$` and not inside single quotes: call `handle_dollar`
 * 4. otherwise: append the character literally
 * 
 * This is the heart of quote-aware expansion.
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
 * Expands the variable references in one word.
 *
 * 1. validate input
 * 2. create empty output string
 * 3. initialize index and quote flags
 * 4. loop over every character in the word
 * 5. call `process_char` for each character
 * 6. return the final expanded string
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
 * Goes through all tokens in the linked list.
 * 
 * For each token whose type is `T_WORD`, it:
 * 1. expands it with `expand_word`
 * 2. frees the old token value
 * 3. replaces it with the expanded value
 * 
 * This is the step that applies expansion across all parsed shell 
 * words before execution.
 */
int	expand_tokens(t_data *data, t_token *tokens)
{
	char	*expanded;

	while (tokens)
	{
		if (tokens->type == T_WORD)
		{
			expanded = expand_word(data, tokens->value);
			if (!expanded)
				return (-1);
			free(tokens->value);
			tokens->value = expanded;
		}
		tokens = tokens->next;
	}
	return (0);
}
