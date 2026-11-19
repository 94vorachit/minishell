/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_utils2.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/10 18:02:41 by vorhansa          #+#    #+#             */
/*   Updated: 2026/11/19 14:26:39 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/**
 * Purpose: reject unsupported startup arguments.
 *
 * How it works:
 * 1. accepts only a single executable invocation
 * 2. prints a usage-like error if extra arguments are passed
 *
 * Return: 1 if valid, 0 if invalid.
 */
int	check_args(int ac, char **av)
{
	(void)av;
	if (ac != 1)
	{
		ft_putstr_fd("minishell: Use ./minishell without arguments", 2);
		return (0);
	}
	return (1);
}

/**
 * Purpose: allocate and initialize the main shell structure.
 *
 * How it works:
 * 1. allocate `t_data`
 * 2. call `init()` to fill shell fields and env list
 * 3. free partial state on failure
 *
 * Return: 0 on success, ERROR on allocation or init failure.
 */
int	setup_data(t_data **data, char **env)
{
	*data = malloc(sizeof(**data));
	if (!*data)
		return (ERROR);
	if (init(*data, env) == -1)
	{
		ft_env_clear(&(*data)->env);
		ft_env_clear(&(*data)->export);
		free(*data);
		return (ERROR);
	}
	return (0);
}

/**
 * Purpose: apply signal status to the shell when Ctrl+C/Ctrl+\ occurs.
 *
 * How it works:
 * 1. check the global signal value
 * 2. convert it to the bash-like exit code `128 + signal`
 * 3. clear the signal flag so the next prompt can continue normally
 *
 * Return: none.
 */
void	apply_signal_status(t_data *data)
{
	if (g_signal == SIGINT || g_signal == SIGQUIT)
	{
		data->status = 128 + g_signal;
		g_signal = 0;
	}
}

/**
 * Purpose: classify the current readline input line.
 *
 * How it works:
 * 1. if readline returned NULL, print `exit` and return EOF state
 * 2. if the line is empty, free it and return skip state
 * 3. otherwise return normal input state
 *
 * Return: 1 for EOF, 2 for an empty line, 0 for normal input.
 */
int	check_line_state(char **line)
{
	if (!*line)
	{
		ft_putstr_fd("exit\n", STDERR);
		return (1);
	}
	if ((*line)[0] == '\0')
	{
		free(*line);
		return (2);
	}
	return (0);
}

/**
 * Purpose: handle EOF or empty readline input and free shell state on EOF.
 *
 * How it works:
 * 1. classify the line with `check_line_state()`
 * 2. if EOF is detected, save the status, free shell memory, and exit
 * 3. if the line is empty, return 2 so the main loop skips it
 * 4. otherwise return 0 so the main loop executes normal input
 *
 * Return: 2 for an empty line, 0 for normal input.
 */
int	handle_input_line(t_data *data, char **line)
{
	int	line_state;
	int	status;

	line_state = check_line_state(line);
	if (line_state == 1)
	{
		status = data->status;
		free_shell(data);
		exit(status);
	}
	if (line_state == 2)
		return (2);
	return (0);
}
