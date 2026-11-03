/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_exit.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/29 16:58:46 by vorhansa          #+#    #+#             */
/*   Updated: 2026/11/03 17:59:41 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/**
 * Checks whether a string is a valid numeric argument for `exit`.
 * 
 * What it does:
 * 1. Rejects `NULL` or empty strings
 * 2. Accepts an optional leading `+` or `-`
 * 3. Verifies every remaining character is a digit
 * 
 * Returns:
 * * 1 if valid
 * * 0 if invalid
 */
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

/**
 * Handles the case where `exit` receives too many arguments.
 */
int	exit_too_many_args(t_data *data)
{
	ft_putstr_fd("minishell: exit: too many arguments\n", STDERR);
	data->status = 1;
	return (1);
}

/**
 * Handles invalid numeric input for `exit`.
 */
int	exit_numeric_required(t_data *data)
{
	ft_putstr_fd("exit\n", STDERR);
	ft_putstr_fd("minishell: exit: numeric argument required\n", STDERR);
	free_shell(data);
	exit(2);
	return (0);
}

/**
 * Main implementation of the `exit` builtin.
 * 
 * What it does:
 * 1. Validates input
 * 2. If there is no argument:
 * * uses the current shell status
 * * frees shell resources
 * * exits with that status
 * 3. If there are 2 or more arguments:
 * * calls exit_too_many_args()
 * 4. If the argument is not numeric:
 * * calls exit_numeric_required()
 * 
 * Otherwise:
 * 1. converts the value to int
 * 2. normalizes negative values
 * 3. prints a terminal termination message
 * 4. frees shell resources
 * 5. exits with code % 256
 */
int	builtin_exit(t_data *data, t_cmd *cmd)
{
	int	code;
	int	status;

	if (!data || !cmd || !cmd->argv || !cmd->argv[0])
		return (1);
	if (!cmd->argv[1])
	{
		status = data->status;
		free_shell(data);
		exit(status);
	}
	if (cmd->argv[2])
		return (exit_too_many_args(data));
	if (!is_numeric_exit_arg(cmd->argv[1]))
		return (exit_numeric_required(data));
	code = ft_atoi(cmd->argv[1]);
	if (code < 0)
		code = 256 + code;
	ft_putstr_fd("The terminal process \"usr/bin\" terminated with exit code: ",
		STDERR);
	ft_putnbr_fd(code, STDERR);
	ft_putstr_fd("\n", STDERR);
	free_shell(data);
	exit(code % 256);
	return (0);
}
