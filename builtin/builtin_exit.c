/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_exit.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/29 16:58:46 by vorhansa          #+#    #+#             */
/*   Updated: 2026/10/29 20:08:34 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

static int	is_numeric_exit_arg(char *arg)
{
	int	idx;

	if (!arg || !*arg)
		return (0);
	idx = 0;
	if (arg[idx] == '+' || arg[idx] == '-')
		idx++;
	if (!arg[idx])
		return (0);
	while (arg[idx])
	{
		if (!ft_isdigit((unsigned char)arg[idx]))
			return (0);
		idx++;
	}
	return (1);
}

int	builtin_exit(t_data *data, t_cmd *cmd)
{
	int	code;

	if (!cmd->argv[1])
	{
		free_shell();
		exit(data->status);
	}
	if (cmd->argv[2])
	{
		ft_putstr_fd("exit\n", STDERR);
		ft_putstr_fd("minishell: exit: too many arguments\n", STDERR);
		data->status = 1;
		return (1);
	}
	if (!is_numeric_exit_arg(cmd->argv[1]))
	{
		ft_putstr_fd("exit\n", STDERR);
		ft_putstr_fd("minishell: exit: numeric argument required\n", STDERR);
		free_shell();
		exit(2);
	}
	code = ft_atoi(cmd->argv[1]);
	if (code < 0)
		code = 256 + code;
	free_shell();
	exit(code % 256);
}
