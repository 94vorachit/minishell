/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/26 15:53:51 by vorhansa          #+#    #+#             */
/*   Updated: 2026/11/20 16:13:08 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/**
 * Purpose: run the main minishell input and execution loop.
 *
 * How it works:
 * 1. validate startup arguments and initialize shell state
 * 2. install signal handlers
 * 3. read input with `readline()` and apply any signal status
 * 4. handle EOF or empty input with `ft_handle_input_line()`
 * 5. execute normal input as one or more command chunks
 * 6. free the current input line and continue the loop
 *
 * Return: the shell status on normal program exit.
 */
int	main(int ac, char **av, char **env)
{
	char	*line;
	t_data	*data;
	t_cmd	cmd;
	int		line_state;

	if (!ft_check_args(ac, av))
		return (0);
	if (ft_setup_data(&data, env) == ERROR)
		return (ERROR);
	ft_sig_init();
	while (1)
	{
		line = readline("minishell > ");
		ft_apply_signal_status(data);
		line_state = ft_handle_input_line(data, &line);
		if (line_state == 2)
			continue ;
		if (ft_handle_multiline_input(data, &cmd, line) == ERROR)
			data->status = ERROR;
		free(line);
	}
}
