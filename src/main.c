/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/26 15:53:51 by vorhansa          #+#    #+#             */
/*   Updated: 2026/11/10 18:37:51 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

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

/**
 * Purpose: main shell loop for interactive input processing.
 *
 * How it works:
 * 1. initialize shell state and signal handlers
 * 2. read one line with readline()
 * 3. handle EOF or empty lines via `handle_input_line()`
 * 4. add valid input to history and execute it
 * 5. clean token list and input buffer each iteration
 *
 * Return: the last shell status when the shell exits.
 */
int	main(int ac, char **av, char **env)
{
	char	*line;
	t_data	*data;
	t_cmd	cmd;
	int		line_state;

	if (!check_args(ac, av))
		return (0);
	if (setup_data(&data, env) == ERROR)
		return (ERROR);
	sig_init();
	while (1)
	{
		line = readline("minishell > ");
		apply_signal_status(data);
		line_state = handle_input_line(data, &line);
		if (line_state == 1)
			return (data->status);
		if (line_state == 2)
			continue ;
		add_history(line);
		execute_token_stream(data, &cmd, line);
		ft_token_clear(&data->tokens);
		free(line);
	}
}
