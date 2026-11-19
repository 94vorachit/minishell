/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/27 20:24:16 by vorhansa          #+#    #+#             */
/*   Updated: 2026/11/19 19:51:49 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/**Reads a quoted section of a word.
 * 
 * Example:
 * * `"hello"`
 * * `'abc'`
 * 
 * How it works:
 * 1. save the quote character (`'` or `"`)
 * 2. add the opening quote to the word
 * 3. move forward until the matching closing quote is found
 * 4. add the closing quote to the word
 * 5. if no closing quote exists, print a syntax error and return `SYNTAX_ERROR`
 * 
 * This allows quoted words to be kept as one token even if they contain spaces. 
 */
int	ft_quote_word(char *line, int *index, char **word)
{
	char	quote;

	quote = line[*index];
	if (ft_add_char(word, quote) == -1)
		return (-1);
	(*index)++;
	while (line[*index] && line[*index] != quote)
	{
		if (ft_add_char(word, line[*index]) == -1)
			return (-1);
		(*index)++;
	}
	if (line[*index] != quote)
	{
		ft_putstr_fd("minishell: unexpected EOF while looking for matching `",
			STDERR);
		ft_putchar_fd(quote, STDERR);
		ft_putstr_fd("'\n", STDERR);
		ft_print_syntax_eof();
		return (SYNTAX_ERROR);
	}
	if (ft_add_char(word, quote) == -1)
		return (-1);
	(*index)++;
	return (0);
}

/**Handles one non-delimiter character while reading a word token.
 *
 * How it works:
 * 1. if current char starts a quote block, delegates to `ft_quote_word`
 * 2. otherwise appends current char literally
 * 3. advances index according to the consumed input
 *
 * Return:
 * * `0` on success
 * * `SYNTAX_ERROR` for quote parsing errors
 * * `-1` on allocation failure
 */
int	read_word_char(char *line, int *index, char **word)
{
	int	result;

	if (line[*index] == '\'' || line[*index] == '"')
	{
		result = ft_quote_word(line, index, word);
		if (result != 0)
			return (result);
		return (0);
	}
	if (ft_add_char(word, line[*index]) == -1)
		return (-1);
	(*index)++;
	return (0);
}

/**Reads one full shell word until a delimiter is reached.
 *
 * Delimiters are:
 * * whitespace
 * * operators (`|`, `<`, `>`)
 * * end of input
 *
 * How it works:
 * 1. loops while current char is part of the same word
 * 2. marks `started` once at least one char belongs to the word
 * 3. delegates per-character logic to `read_word_char`
 * 4. propagates helper return codes directly to the caller
 *
 * This function controls word boundaries; character details are handled
 * by `read_word_char`.
 */
int	ft_read_word(char *line, int *index, char **word, int *started)
{
	int	result;

	while (line[*index])
	{
		if (ft_isspace(line[*index]) || ft_isoperator(line[*index]))
			break ;
		*started = 1;
		result = read_word_char(line, index, word);
		if (result != 0)
			return (result);
	}
	return (0);
}

/**Creates a word token from a segment of the input.
 * 
 * Steps:
 * 1. initialize the word as an empty string
 * 2. call `ft_read_word`
 * 3. if the word was empty, return `0`
 * 4. otherwise add the word as a `T_WORD` token
 * 
 * This is the function that turns a raw text fragment into a proper token. 
 */
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
