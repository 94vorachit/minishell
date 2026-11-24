/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_types.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/28 17:14:07 by vorhansa          #+#    #+#             */
/*   Updated: 2026/11/20 16:27:24 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/**Checks whether the current character is |.
 * 
 * If yes:
 * * sets the token type to `T_REDIR_IN`
 * * returns a duplicated string `"|"`
 * 
 * This is a little unusual because it assigns `T_REDIR_IN` instead of `T_PIPE`, 
 * which is likely a bug or mismatch in the current code. 
 * In normal shell parsing, `|` should use `T_PIPE`.
 */
char	*ft_set_pipe_token(char *line, int *index, t_token_type *type)
{
	if (line[*index] == '|')
	{
		*type = T_PIPE;
		return (ft_strdup("|"));
	}
	return (NULL);
}

/**Checks whether the current input is `<<`.
 * 
 * If yes:
 * * sets the token type to `T_HEREDOC`
 * * advances the index by 1 to skip the second `<`
 * * returns the duplicated string `"<<"`
 * 
 * This handles the two-character heredoc operator.
 */
char	*ft_set_heredoc_token(char *line, int *index, t_token_type *type)
{
	if (line[*index] == '<' && line[*index + 1] == '<')
	{
		*type = T_HEREDOC;
		(*index)++;
		return (ft_strdup("<<"));
	}
	return (NULL);
}

/**Checks whether the current input is `>>`.
 * 
 * If yes:
 * * sets the token type to `T_APPEND`
 * * advances the index by 1
 * * returns the duplicated string `">>"`
 * 
 * This handles output redirection in append mode. */
char	*ft_set_append_token(char *line, int *index, t_token_type *type)
{
	if (line[*index] == '>' && line[*index + 1] == '>')
	{
		*type = T_APPEND;
		(*index)++;
		return (ft_strdup(">>"));
	}
	return (NULL);
}

/**Checks whether the current character is `<`.
 *
 * If yes:
 * * sets the token type to `T_REDIR_IN`
 * * returns the duplicated string `"<"`
 * 
 * This handles input redirection. 
 * */
char	*ft_set_redir_in_token(char *line, int *index, t_token_type *type)
{
	if (line[*index] == '<')
	{
		*type = T_REDIR_IN;
		return (ft_strdup("<"));
	}
	return (NULL);
}

/**Checks whether the current character is `>`.
 * 
 * If yes:
 * * sets the token type to `T_REDIR_OUT`
 * * returns the duplicated string `">"`
 * 
 * This handles output redirection. 
 * */
char	*ft_set_redir_out_token(char *line, int *index, t_token_type *type)
{
	if (line[*index] == '>')
	{
		*type = T_REDIR_OUT;
		return (ft_strdup(">"));
	}
	return (NULL);
}
