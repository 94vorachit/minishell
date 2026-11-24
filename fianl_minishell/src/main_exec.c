/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_exec.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/19 13:52:48 by vorhansa          #+#    #+#             */
/*   Updated: 2026/11/23 14:46:24 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/**
 * Purpose: Execute one single line or command chunk.
 * 
 * How it works:
 * 1. Ignores the chunk if it is `NULL` or empty.
 * 2. Adds it to the readline command history.
 * 3. Calls `ft_exec_token_stream()` to tokenize and execute the chunk.
 * 4. Clears the resulting token list to prepare for the next chunk.
 */
int	ft_exec_input_chunk(t_data *data, t_cmd *cmd, char *chunk)
{
	if (!chunk || chunk[0] == '\0')
		return (SUCCESS);
	add_history(chunk);
	ft_exec_token_stream(data, cmd, chunk);
	ft_token_clear(&data->tokens);
	return (SUCCESS);
}

/**
 * Purpose: execute multiline input without re-executing heredoc body lines.
 *
 * How it works:
 * 1. split the line by '\n' into chunks
 * 2. execute heredoc command chunks first
 * 3. mark heredoc body + delimiter lines as skipped
 * 4. execute only the remaining chunks
 */
int	ft_handle_multiline_input(t_data *data, t_cmd *cmd, char *line)
{
	char	**chunks;
	int		*skip;
	int		chunk_count;

	if (!ft_strchr(line, '\n'))
		return (ft_exec_input_chunk(data, cmd, line));
	chunks = ft_split(line, '\n');
	if (!chunks)
		return (ERROR);
	chunk_count = ft_count_chunks(chunks);
	skip = ft_calloc(chunk_count, sizeof(int));
	if (!skip)
	{
		ft_free_split(chunks);
		return (ERROR);
	}
	ft_exec_heredoc_chunks_first(data, cmd, chunks, skip);
	ft_exec_remaining_chunks(data, cmd, chunks, skip);
	free(skip);
	ft_free_split(chunks);
	return (SUCCESS);
}

/**
 * Purpose: execute a pipeline built from the parsed token list.
 *
 * How it works:
 * 1. build a list of `t_cmd` nodes from the token stream
 * 2. run the pipeline executor
 * 3. set the shell status from the result
 * 4. free the temporary command list
 *
 * Return: always 0 for the current control flow.
 */
int	ft_exec_pipe_cmds(t_data *data)
{
	t_cmd	*cmds;
	int		build_ret;

	cmds = NULL;
	build_ret = ft_build_pipe_cmds(data->tokens, &cmds);
	if (build_ret == 0)
		data->status = ft_exec_pipe(data, cmds);
	else if (build_ret == ERROR)
		data->status = ERROR;
	else
		data->status = SYNTAX_ERROR;
	ft_free_cmd_list(&cmds);
	return (0);
}

/**
 * Purpose: execute a single command parsed from the current token stream.
 *
 * How it works:
 * 1. build a `t_cmd` from `data->tokens`
 * 2. run the command with `ft_exec_simple_cmd()`
 * 3. free the command's argv/redirection memory
 * 4. store the status in `data->status`
 *
 * Return: always 0 in this caller pattern.
 */
int	ft_run_simple_cmd_from_tokens(t_data *data, t_cmd *cmd)
{
	int	build_ret;

	build_ret = ft_build_simple_cmd(data->tokens, cmd);
	if (build_ret == 0)
	{
		data->status = ft_exec_simple_cmd(data, cmd);
		ft_free_cmd(cmd);
	}
	else
	{
		ft_free_cmd(cmd);
		if (build_ret == ERROR)
			data->status = ERROR;
		else
			data->status = SYNTAX_ERROR;
	}
	return (0);
}

/**
 * Purpose: parse, validate, expand, and dispatch one input stream.
 *
 * How it works:
 * 1. convert the raw line into tokens
 * 2. validate shell syntax
 * 3. expand variables and quotes
 * 4. choose pipeline or simple command execution
 *
 * Return: 0 on success or a controlled status code from the subroutine.
 */
int	ft_exec_token_stream(t_data *data, t_cmd *cmd, char *line)
{
	int	syntax_ret;

	if (ft_lexer(line, &data->tokens) != 0)
	{
		data->status = SYNTAX_ERROR;
		return (0);
	}
	syntax_ret = ft_validate_syntax(data->tokens);
	if (syntax_ret != SUCCESS)
	{
		data->status = syntax_ret;
		return (0);
	}
	if (ft_expand_tokens(data, &data->tokens) == -1)
	{
		data->status = ERROR;
		return (0);
	}
	if (ft_has_pipe(data->tokens))
		return (ft_exec_pipe_cmds(data));
	return (ft_run_simple_cmd_from_tokens(data, cmd));
}
