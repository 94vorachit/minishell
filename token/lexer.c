/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/27 20:24:12 by vorhansa          #+#    #+#             */
/*   Updated: 2026/10/27 20:46:17 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	ft_print_tokens(t_token *tokens)
{
    while (tokens)
    {
        printf("value = [%s], type = %d\n",
            tokens->value, tokens->type);
        tokens = tokens->next;
    }
}

char	*ft_operator_value(char *line, int *index, t_token_type *type)
{
    *type = T_PIPE;
    if (line[*index] == '|')
        return (ft_strdup("|"));
    if (line[*index] == '<' && line[*index + 1] == '<')
    {
        *type = T_HEREDOC;
        (*index)++;
        return (ft_strdup("<<"));
    }
    if (line[*index] == '>' && line[*index + 1] == '>')
    {
        *type = T_APPEND;
        (*index)++;
        return (ft_strdup(">>"));
    }
    if (line[*index] == '<')
    {
        *type = T_REDIR_IN;
        return (ft_strdup("<"));
    }
    if (line[*index] == '>')
    {
        *type = T_REDIR_OUT;
        return (ft_strdup(">"));
    }
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
    