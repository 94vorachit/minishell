/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/03 14:15:04 by vorhansa          #+#    #+#             */
/*   Updated: 2026/11/09 20:05:22 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/**
 * Checks whether the token list contains a pipe
 * 
 * Behavior:
 * 1. walks the token list from the start
 * 2. if it sees `T_PIPE`, returns `1`
 * 3. otherwise returns `0`
 * 
 * This is a simple scanner used to detect whether the input is a pipeline.
 */
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

/**
 * Appends a command node to the end of the pipeline list
 * 
 * Behavior:
 * 1. if the list is empty, it becomes the head
 * 2. otherwise, it walks to the last node and sets `cur->next = new_cmd`
 * 
 * This is the insertion helper for the linked list of command segments.
 */
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

/**
 * Builds one command from the tokens before a pipe
 * 
 * Behavior:
 * 1. rejects an empty segment when `start == pipe_pos`
 * 2. allocates a new `t_cmd`
 * 3. calls `build_simple_cmd(start, new_cmd)` to parse arguments 
 * and redirections
 * 4. appends it to the list with `cmd_add_back`
 * 
 * This handles the left side of a pipe segment.
 */
int	add_pipeline_segment(t_token *start, t_token *pipe_pos, t_cmd **head)
{
	t_cmd	*new_cmd;
	int		ret;

	if (pipe_pos == start)
		return (-1);
	new_cmd = ft_calloc(1, sizeof(*new_cmd));
	if (!new_cmd)
		return (-1);
	ret = build_simple_cmd(start, new_cmd);
	if (ret == -1)
	{
		ft_free_cmd(new_cmd);
		free(new_cmd);
		return (-1);
	}
	if (ret == ERROR)
	{
		ft_free_cmd(new_cmd);
		free(new_cmd);
		return (ERROR);
	}
	cmd_add_back(head, new_cmd);
	return (0);
}

/**
 * Builds the final command after the last pipe
 * 
 * Behavior:
 * 1. allocates a new `t_cmd`
 * 2. parses the remaining tokens with `build_simple_cmd`
 * 3. adds it to the command list
 * 
 * This handles the final segment after the last `|`.
 */
int	append_last_pipeline_cmd(t_token *start, t_cmd **head)
{
	t_cmd	*new_cmd;
	int		ret;

	new_cmd = ft_calloc(1, sizeof(*new_cmd));
	if (!new_cmd)
	{
		ft_free_cmd_list(head);
		return (-1);
	}
	ret = build_simple_cmd(start, new_cmd);
	if (ret == -1)
	{
		free(new_cmd);
		ft_free_cmd_list(head);
		return (-1);
	}
	if (ret == ERROR)
	{
		free(new_cmd);
		ft_free_cmd_list(head);
		return (ERROR);
	}
	cmd_add_back(head, new_cmd);
	return (0);
}

/**
 * Main entry point that splits the whole token list into commands
 * 
 * Behavior:
 * 1. initializes `*head = NULL`
 * 2. `start = tokens`
 * 3. walks token by token
 * 4. when a `T_PIPE` is found:
 * * build the segment before that pipe
 * * move `start` to the token after the pipe
 * * continue scanning
 * 5. after the loop, build the last remaining command with 
 * `append_last_pipeline_cmd`
 * 
 * So this function is the “pipeline splitter.”
 */
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
