/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/26 15:53:51 by vorhansa          #+#    #+#             */
/*   Updated: 2026/11/19 13:53:15 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

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
		if (handle_multiline_input(data, &cmd, line) == ERROR)
			data->status = ERROR;
		free(line);
	}
}
