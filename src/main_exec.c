/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_exec.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/19 13:52:48 by vorhansa          #+#    #+#             */
/*   Updated: 2026/11/19 16:22:58 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/**
 * Purpose: Execute one single line or command chunk.
 * 
 * How it works:
 * 1. Ignores the chunk if it is `NULL` or empty.
 * 2. Adds it to the readline command history.
 * 3. Calls `execute_token_stream()` to tokenize and execute the chunk.
 * 4. Clears the resulting token list to prepare for the next chunk.
 */
int	execute_input_chunk(t_data *data, t_cmd *cmd, char *chunk)
{
	if (!chunk || chunk[0] == '\0')
		return (SUCCESS);
	add_history(chunk);
	execute_token_stream(data, cmd, chunk);
	ft_token_clear(&data->tokens);
	return (SUCCESS);
}

/**
 * Purpose: Support input containing multiple lines, 
 * such as piped input or pasted text.
 * 
 * How it works:
 * 1. Checks whether line contains a newline.
 * 2. If it is a single-line input, delegates directly to execute_input_chunk().
 * 3. Otherwise, splits the input using '\n'.
 * 4. Executes each resulting chunk independently.
 * 5. Frees the split array after execution.
 */
static char	*get_heredoc_delimiter(char *chunk)
{
	char	*pos;
	char	*end;

	pos = ft_strstr(chunk, "<<");
	if (!pos)
		return (NULL);
	pos += 2;
	while (*pos && ft_isspace((unsigned char)*pos))
		pos++;
	if (!*pos)
		return (NULL);
	end = pos;
	while (*end && !ft_isspace((unsigned char)*end)
		&& *end != '<' && *end != '>' && *end != '|')
		end++;
	return (ft_substr(pos, 0, end - pos));
}

int	handle_multiline_input(t_data *data, t_cmd *cmd, char *line)
{
	char	**chunks;
	char	*delimiter;
	int		i;

	if (!ft_strchr(line, '\n'))
		return (execute_input_chunk(data, cmd, line));
	chunks = ft_split(line, '\n');
	if (!chunks)
		return (ERROR);
	delimiter = get_heredoc_delimiter(chunks[0]);
	if (delimiter)
		execute_input_chunk(data, cmd, chunks[0]);
	else
	{
		i = 0;
		while (chunks[i])
		{
			execute_input_chunk(data, cmd, chunks[i]);
			i++;
		}
		ft_free_split(chunks);
		return (SUCCESS);
	}
	i = 1;
	while (chunks[i])
	{
		if (ft_strcmp(chunks[i], delimiter) == 0)
		{
			free(delimiter);
			delimiter = NULL;
			break ;
		}
		i++;
	}
	free(delimiter);
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
int	execute_pipeline_cmds(t_data *data)
{
	t_cmd	*cmds;
	int		build_ret;

	cmds = NULL;
	build_ret = build_pipeline_cmds(data->tokens, &cmds);
	if (build_ret == 0)
		data->status = execute_pipeline(data, cmds);
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
 * 2. run the command with `execute_simple_cmd()`
 * 3. free the command's argv/redirection memory
 * 4. store the status in `data->status`
 *
 * Return: always 0 in this caller pattern.
 */
int	run_simple_cmd_from_tokens(t_data *data, t_cmd *cmd)
{
	int	build_ret;

	build_ret = build_simple_cmd(data->tokens, cmd);
	if (build_ret == 0)
	{
		data->status = execute_simple_cmd(data, cmd);
		ft_free_cmd(cmd);
	}
	else if (build_ret == ERROR)
		data->status = ERROR;
	else
		data->status = SYNTAX_ERROR;
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
int	execute_token_stream(t_data *data, t_cmd *cmd, char *line)
{
	if (lexer(line, &data->tokens) != 0)
	{
		data->status = SYNTAX_ERROR;
		return (0);
	}
	if (ft_validate_syntax(data->tokens) != SUCCESS)
	{
		data->status = SYNTAX_ERROR;
		return (0);
	}
	if (expand_tokens(data, &data->tokens) == -1)
	{
		data->status = ERROR;
		return (0);
	}
	if (ft_has_pipe(data->tokens))
		return (execute_pipeline_cmds(data));
	return (run_simple_cmd_from_tokens(data, cmd));
}
