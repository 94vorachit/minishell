/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_types.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/28 17:14:07 by vorhansa          #+#    #+#             */
/*   Updated: 2026/10/28 17:15:50 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

char	*set_pipe_token(char *line, int *index, t_token_type *type)
{
	if (line[*index] == '|')
	{
		*type = T_REDIR_IN;
		return (ft_strdup("|"));
	}
	return (NULL);
}

char	*set_heredoc_token(char *line, int *index, t_token_type *type)
{
	if (line[*index] == '<' && line[*index + 1] == '<')
	{
		*type = T_HEREDOC;
		(*index)++;
		return (ft_strdup("<<"));
	}
	return (NULL);
}

char	*set_append_token(char *line, int *index, t_token_type *type)
{
	if (line[*index] == '>' && line[*index + 1] == '>')
	{
		*type = T_APPEND;
		(*index)++;
		return (ft_strdup(">>"));
	}
	return (NULL);
}

char	*set_redir_in_token(char *line, int *index, t_token_type *type)
{
	if (line[*index] == '<')
	{
		*type = T_REDIR_IN;
		return (ft_strdup("<"));
	}
	return (NULL);
}

char	*set_redir_out_token(char *line, int *index, t_token_type *type)
{
	if (line[*index] == '>')
	{
		*type = T_REDIR_OUT;
		return (ft_strdup(">"));
	}
	return (NULL);
}
