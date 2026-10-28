/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/27 20:24:16 by vorhansa          #+#    #+#             */
/*   Updated: 2026/10/28 18:33:45 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/**Checks whether a token type is a redirection.
 * 
 * It returns true for:
 * * `T_REDIR_IN`
 * * `T_REDIR_OUT`
 * * `T_APPEND`
 * * `T_HEREDOC`
 * 
 * This is used later to decide whether a token is a redirection operator.
 */
int	ft_isredirection(t_token_type type)
{
	return (type == T_REDIR_IN || type == T_REDIR_OUT
		|| type == T_APPEND || type == T_HEREDOC);
}

/** Checks whether a character is one of the shell operators:
 * * `|`
 * * `<`
 * * `>`
 * 
 * This is used while scanning a word to stop reading when 
 * the parser hits an operator.
 */
int	ft_isoperator(char c)
{
	return (c == '|' || c == '<' || c == '>');
}

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
		ft_putstr_fd(
			"minishell: syntax error: unexpected end of file\n", STDERR);
		return (SYNTAX_ERROR);
	}
	if (ft_add_char(word, quote) == -1)
		return (-1);
	(*index)++;
	return (0);
}

/**Reads a normal word from the input.
 * 
 * It continues until:
 * * it reaches a space
 * * it reaches an operator (`|`, `<`, `>`)
 * * or the string ends
 * 
 * Inside the loop:
 * * if it sees a quote, it calls ft_quote_word
 * * otherwise, it copies the character into the word buffer
 * 
 * This is the main builder for a word token. 
 */
int	ft_read_word(char *line, int *index, char **word, int *started)
{
	int	result;

	while (line[*index])
	{
		if (ft_isspace(line[*index]) || ft_isoperator(line[*index]))
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
