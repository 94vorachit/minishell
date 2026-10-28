/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/27 20:24:12 by vorhansa          #+#    #+#             */
/*   Updated: 2026/10/28 17:49:18 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/**
 * Prints the whole token list.
 * 
 * It loops through the linked list and prints:
 * * token value
 * * token type
 * 
 * Example output:
 * * value = [|], type = 0
 * 
 * This is useful for debugging the lexer.
 */
void	ft_print_tokens(t_token *tokens)
{
	while (tokens)
	{
		printf("value = [%s], type = %d\n",
			tokens->value, tokens->type);
		tokens = tokens->next;
	}
}

/**
 * Detects and creates the value string for operators.
 * 
 * It checks in order:
 * * pipe |
 * * heredoc <<
 * * append >>
 * * input redirect <
 * * output redirect >
 * It uses helper functions to decide which operator matches 
 * the current character.
 * 
 * Important behavior:
 * * << and >> consume two characters
 * * the index is advanced in the helper functions
 * * it returns the string for the operator, such as |, <, >>, etc.
 */
char	*ft_operator_value(char *line, int *index, t_token_type *type)
{
	char	*value;

	value = set_pipe_token(line, index, type);
	if (value)
		return (value);
	value = set_heredoc_token(line, index, type);
	if (value)
		return (value);
	value = set_append_token(line, index, type);
	if (value)
		return (value);
	value = set_redir_in_token(line, index, type);
	if (value)
		return (value);
	value = set_redir_out_token(line, index, type);
	if (value)
		return (value);
	return (NULL);
}

int	ft_lex_operator(char *line, int *index, t_token **tokens)
{
	char			*value;
	t_token_type	type;

	value = ft_operator_value(line, index, &type);
	if (!value)
		return (-1);
	(*index)++;
	if (ft_add_token(tokens, value, type) == -1)
		return (-1);
	return (0);
}

int	lexer(char *line, t_token **tokens)
{
	int	index;
	int	result;

	index = 0;
	*tokens = NULL;
	while (line[index])
	{
		while (line[index] && ft_isspace(line[index]))
			index++;
		if (!line[index])
			break ;
		if (ft_isoperator(line[index]))
			result = ft_lex_operator(line, &index, tokens);
		else
			result = ft_lex_word(line, &index, tokens);
		if (result != 0)
		{
			ft_token_clear(tokens);
			return (result);
		}
	}
	return (0);
}
