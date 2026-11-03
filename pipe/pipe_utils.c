/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/03 14:15:04 by vorhansa          #+#    #+#             */
/*   Updated: 2026/11/03 14:45:51 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	cmd_add_back(t_cmd **head, t_cmd *new_cmd)
{
	t_cmd	*cur;

	if (!head || !new_cmd)
		return ;
	if (!*head)
	{
		*head = new_cmd;
		return ;
	}
	cur = *head;
	while (cur->next)
		cur = cur->next;
	cur->next = new_cmd;
}

int	ft_has_pipe(t_token *tokens)
{
	t_token	*cur;

	cur = tokens;
	while (cur)
	{
		if (cur->type == T_PIPE)
			return (1);
		cur = cur->next;
	}
	return (0);
}

int	add_pipeline_segment(t_token *start, t_token *pipe_pos, t_cmd **head)
{
	t_cmd	*new_cmd;

	if (pipe_pos == start)
		return (-1);
	new_cmd = ft_calloc(1, sizeof(*new_cmd));
	if (!new_cmd)
		return (-1);
	if (build_simple_cmd(start, new_cmd) == -1)
	{
		free(new_cmd);
		return (-1);
	}
	cmd_add_back(head, new_cmd);
	return (0);
}

int	append_last_pipeline_cmd(t_token *start, t_cmd **head)
{
	t_cmd	*new_cmd;

	new_cmd = ft_calloc(1, sizeof(*new_cmd));
	if (!new_cmd)
	{
		ft_free_cmd_list(head);
		return (-1);
	}
	if (build_simple_cmd(start, new_cmd) == -1)
	{
		free(new_cmd);
		ft_free_cmd_list(head);
		return (-1);
	}
	cmd_add_back(head, new_cmd);
	return (0);
}

int	build_pipeline_cmds(t_token *tokens, t_cmd **head)
{
	t_token	*cur;
	t_token	*start;

	if (!head || !tokens)
		return (-1);
	*head = NULL;
	start = tokens;
	cur = tokens;
	while (cur)
	{
		if (cur->type == T_PIPE)
		{
			if (add_pipeline_segment(start, cur, head) == -1)
				return (-1);
			start = cur->next;
			cur = start;
			continue ;
		}
		cur = cur->next;
	}
	if (!start)
		return (0);
	return (append_last_pipeline_cmd(start, head));
}
