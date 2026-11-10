/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_utils2.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/10 18:02:41 by vorhansa          #+#    #+#             */
/*   Updated: 2026/11/10 18:37:51 by vorhansa         ###   ########.fr       */
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
 * Purpose: decide whether the current input line should be ignored, exit, or continue.
 *
 * How it works:
 * 1. if `readline()` returns NULL, print `exit` and clean shell memory
 * 2. if the line is empty, free it and skip processing
 * 3. otherwise accept the line as normal input
 *
 * Return: 1 to exit, 2 to skip the current iteration, 0 to continue.
 */
int	check_line_state(t_data *data, char **line)
{
	if (!*line)
	{
		ft_putstr_fd("exit\n", STDERR);
		free_shell(data);
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
 * Purpose: wrap the shell line-state handling in one named helper.
 *
 * How it works:
 * 1. call `check_line_state()`
 * 2. if exit was requested, return 1
 * 3. if the line should be skipped, return 2
 * 4. otherwise return 0 to keep processing
 *
 * Return: 1 for exit, 2 for skip, 0 for continue processing.
 */
int	handle_input_line(t_data *data, char **line)
{
	int	line_state;

	line_state = check_line_state(data, line);
	if (line_state == 1)
	{
		free_shell(data);
		return (1);
	}
	if (line_state == 2)
		return (2);
	return (0);
}
