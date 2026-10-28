/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_redir.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/29 02:18:49 by vorhansa          #+#    #+#             */
/*   Updated: 2026/10/29 02:19:17 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	set_input_file(t_cmd *cmd, char *value)
{
	free(cmd->infile);
	cmd->infile = value;
}

void	set_output_file(t_cmd *cmd, char *value, int append)
{
	free(cmd->outfile);
	cmd->outfile = value;
	cmd->append = append;
}

void	assign_redirection(t_cmd *cmd, t_token_type type, char *value)
{
	if (type == T_REDIR_IN)
		set_input_file(cmd, value);
	else if (type == T_REDIR_OUT)
		set_output_file(cmd, value, 0);
	else if (type == T_APPEND)
		set_output_file(cmd, value, 1);
	else
		free(value);
}

int	set_redirection(t_cmd *cmd, t_token *token)
{
	char	*value;

	if (!token->next || token->next->type != T_WORD)
		return (-1);
	if (token->type == T_HEREDOC)
		return (0);
	value = ft_strdup(token->next->value);
	if (!value)
		return (-1);
	assign_redirection(cmd, token->type, value);
	return (0);
}

int	parse_redirections(t_token *tokens, t_cmd *cmd)
{
	t_token	*cur;

	cur = tokens;
	while (cur && cur->type != T_PIPE)
	{
		if (ft_isredirection(cur->type)
			&& set_redirection(cmd, cur) == -1)
			return (-1);
		if (ft_isredirection(cur->type))
			cur = cur->next;
		cur = cur->next;
	}
	return (0);
}
