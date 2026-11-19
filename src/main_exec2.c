/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_exec2.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/19 16:42:01 by vorhansa          #+#    #+#             */
/*   Updated: 2026/11/19 17:04:33 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/**
 * Purpose: count how many chunks are in a split multiline buffer.
 */
int	count_chunks(char **chunks)
{
	int	i;

	i = 0;
	while (chunks[i])
		i++;
	return (i);
}

/**
 * Purpose: mark heredoc body lines and the delimiter line as skipped.
 *
 * The caller executes the heredoc command itself, so the body must not
 * be executed later as standalone commands.
 */
void	mark_heredoc_body(char **chunks, int *skip, int start, char *delimiter)
{
	int	i;

	i = start + 1;
	while (chunks[i] && ft_strcmp(chunks[i], delimiter) != 0)
	{
		skip[i] = 1;
		i++;
	}
	if (chunks[i] && ft_strcmp(chunks[i], delimiter) == 0)
		skip[i] = 1;
}

/**
 * Purpose: execute chunks that start heredocs before other chunks.
 *
 * This ensures heredoc input is consumed first in multiline flows.
 */
void	execute_heredoc_chunks_first(t_data *data, t_cmd *cmd,
	char **chunks, int *skip)
{
	char	*delimiter;
	int		i;

	i = 0;
	while (chunks[i])
	{
		delimiter = get_heredoc_delimiter(chunks[i]);
		if (delimiter)
		{
			execute_input_chunk(data, cmd, chunks[i]);
			skip[i] = 1;
			mark_heredoc_body(chunks, skip, i, delimiter);
			free(delimiter);
		}
		i++;
	}
}

/**
 * Purpose: execute chunks that were not consumed by heredoc processing.
 */
void	execute_remaining_chunks(t_data *data, t_cmd *cmd,
	char **chunks, int *skip)
{
	int	i;

	i = 0;
	while (chunks[i])
	{
		if (!skip[i])
			execute_input_chunk(data, cmd, chunks[i]);
		i++;
	}
}
