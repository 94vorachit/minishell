/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/27 20:24:16 by vorhansa          #+#    #+#             */
/*   Updated: 2026/10/27 20:41:52 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	ft_add_token(t_token **tokens, char *value, t_token_type type)
{
	t_token	*new_token;

	new_token = ft_token_new(value, type);
	if (!new_token)
	{
		free(value);
		return (-1);
	}
	ft_token_add_back(tokens, new_token);
	return (0);
}

int	ft_quote_word(char *line, int *index, char **word)
{
	char	quote;

	quote = line[*index];
	(*index)++;
	while (line[*index] && line[*index] != quote)
	{
		if (ft_add_char(word, line[*index]) == -1)
			return (-1);
		(*index)++;
	}
	if (line[*index] != quote)
	{
		ft_putstr_fd(
			"minishell: syntax error: unexpected end of file\n", STDERR);
		return (SYNTAX_ERROR);
	}
	(*index)++;
	return (0);
}

// int	ft_lex_word(char *line, int *index, t_token **tokens)
// {
//     char	*word;
//     int		started;
//     int		result;

//     word = ft_strdup("");
//     if (!word)
//         return (-1);
//     started = 0;
//     while (line[*index])
//     {
//         if (ft_isspace(line[*index]) || ft_isoperator(line[*index]))
//             break ;
//         started = 1;
//         if (line[*index] == '\'' || line[*index] == '"')
//         {
//             result = quote_word(line, index, &word);
//             if (result != 0)
//             {
//                 free(word);
//                 return (result);
//             }
//         }
//         else
//         {
//             if (add_char(&word, line[*index]) == -1)
//             {
//                 free(word);
//                 return (-1);
//             }
//             (*index)++;
//         }
//     }
//     if (started == 0)
//     {
//         free(word);
//         return (0);
//     }
//     if (add_token(tokens, word, T_WORD) == -1)
//         return (-1);
//     return (0);
// }

int	ft_read_word(char *line, int *index, char **word, int *started)
{
    int	result;

    while (line[*index])
    {
        if (ft_isspace(line[*index])
            || ft_isoperator(line[*index]))
            break ;
        *started = 1;
        if (line[*index] == '\'' || line[*index] == '"')
        {
            result = ft_quote_word(line, index, word);
            if (result != 0)
                return (result);
        }
        else
        {
            if (ft_add_char(word, line[*index]) == -1)
                return (-1);
            (*index)++;
        }
    }
    return (0);
}

int	ft_lex_word(char *line, int *index, t_token **tokens)
{
    char	*word;
    int		started;
    int		result;

    word = ft_strdup("");
    if (!word)
        return (-1);
    started = 0;
    result = ft_read_word(line, index, &word, &started);
    if (result != 0)
    {
        free(word);
        return (result);
    }
    if (!started)
    {
        free(word);
        return (0);
    }
    if (ft_add_token(tokens, word, T_WORD) == -1)
        return (-1);
    return (0);
}
