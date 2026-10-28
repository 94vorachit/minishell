/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/28 15:05:12 by vorhansa          #+#    #+#             */
/*   Updated: 2026/10/28 16:11:57 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	is_var_start(char c)
{
	return (ft_isalpha((unsigned char)c) || c == '_');
}

int	is_var_char(char c)
{
	return (ft_isalnum((unsigned char)c) || c == '_');
}

int	is_var_name_char(char c)
{
	return (ft_isalpha((unsigned char)c) || c == '_');
}

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

char	*append_char(char *result, char c)
{
	char	part[2];

	part[0] = c;
	part[1] = '\0';
	return (append_text(result, part));
}

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

int	handle_single_quote(int *in_single, size_t *i)
{
	*in_single = !(*in_single);
	(*i)++;
	return (0);
}

int	handle_double_quote(int *in_double, size_t *i)
{
	*in_double = !(*in_double);
	(*i)++;
	return (0);
}

int	append_literal_char(char **result, const char *word, size_t *i)
{
	*result = append_char(*result, word[*i]);
	if (!*result)
		return (-1);
	(*i)++;
	return (0);
}

int	process_char(t_exp_state *state)
{
	if (state->word[*(state->i)] == '\'' && !(state->in_double))
		return (handle_single_quote(&state->in_single, state->i));
	if (state->word[*(state->i)] == '"' && !(state->in_single))
		return (handle_double_quote(&state->in_double, state->i));
	if (state->word[*(state->i)] == '$' && !(state->in_single))
	{
		if (handle_dollar(state->data, state->word, state->i, state->result) != 0)
			return (-1);
		return (0);
	}
	return (append_literal_char(state->result, state->word, state->i));
}

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
