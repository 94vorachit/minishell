#include "../includes/minishell.h"

static int	is_var_start(char c)
{
	return (ft_isalpha((unsigned char)c) || c == '_');
}

static int	is_var_char(char c)
{
	return (ft_isalnum((unsigned char)c) || c == '_');
}

static char	*append_text(char *result, const char *text)
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

static char	*append_char(char *result, char c)
{
	char	part[2];

	part[0] = c;
	part[1] = '\0';
	return (append_text(result, part));
}

static char	*append_variable(char *result, t_data *data,
		const char *word, size_t *index)
{
	size_t	start;
	char	*name;
	char	*value;
	char	*status;

	if (word[*index + 1] == '?')
	{
		status = ft_itoa(data->status);
		if (!status)
		{
			free(result);
			return (NULL);
		}
		result = append_text(result, status);
		free(status);
		*index += 2;
		return (result);
	}
	if (!is_var_start(word[*index + 1]))
		return (append_char(result, word[(*index)++]));
	start = *index + 1;
	*index = start;
	while (is_var_char(word[*index]))
		(*index)++;
	name = ft_substr(word, start, *index - start);
	if (!name)
	{
		free(result);
		return (NULL);
	}
	value = env_get(data->env, name);
	free(name);
	return (append_text(result, value));
}

char	*expand_word(t_data *data, const char *word)
{
	char	*result;
	size_t	i;
	int		in_single;
	int		in_double;

	if (!data || !word)
		return (NULL);
	result = ft_strdup("");
	if (!result)
		return (NULL);
	i = 0;
	in_single = 0;
	in_double = 0;
	while (word[i])
	{
		if (word[i] == '\'' && !in_double)
			in_single = !in_single;
		else if (word[i] == '"' && !in_single)
			in_double = !in_double;
		else if (word[i] == '$' && !in_single)
			result = append_variable(result, data, word, &i);
		else
			result = append_char(result, word[i++]);
		if (!result)
			return (NULL);
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
