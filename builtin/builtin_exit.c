/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_exit.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/29 16:58:46 by vorhansa          #+#    #+#             */
/*   Updated: 2026/10/30 17:40:38 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	is_numeric_exit_arg(char *arg)
{
	int	i;

	if (!arg || !*arg)
		return (0);
	i = 0;
	if (arg[i] == '+' || arg[i] == '-')
		i++;
	if (!arg[i])
		return (0);
	while (arg[i])
	{
		if (!ft_isdigit((unsigned char)arg[i]))
			return (0);
		i++;
	}
	return (1);
}

int	builtin_exit(t_data *data, t_cmd *cmd)
{
	int	code;
	int	status;

	if (!data || !cmd || !cmd->argv || !cmd->argv[0])
		return (1);
	if (!cmd->argv[1])
	{
		status = data->status;
		free_shell();
		exit(status);
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
	ft_putstr_fd("The terminal process \"usr/bin\" terminated with exit code: ", STDERR);
	ft_putnbr_fd(code, STDERR);
	ft_putstr_fd("\n", STDERR);
	free_shell();
	exit(code % 256);
}
