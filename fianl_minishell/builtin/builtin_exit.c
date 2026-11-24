/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_exit.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/20 14:43:00 by vorhansa          #+#    #+#             */
/*   Updated: 2026/11/23 16:38:26 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/**
 * Purpose: parse an `exit` numeric argument into a signed long long.
 *
 * How it works:
 * 1. initializes parsing state (`i`, `sign`, `acc`).
 * 2. consumes optional leading `+` or `-` and stores sign.
 * 3. computes the allowed absolute limit for signed 64-bit range.
 * 4. accumulates digits and checks overflow before each multiply/add.
 * 5. writes the parsed value to `out` and returns success/failure.
 */
int	ft_parse_exit_ll(char *arg, long long *out)
{
	int					i;
	int					sign;
	unsigned long long	acc;
	unsigned long long	limit;

	i = 0;
	sign = 1;
	acc = 0;
	if (arg[i] == '+' || arg[i] == '-')
		sign = 1 - 2 * (arg[i++] == '-');
	limit = (unsigned long long)LLONG_MAX + (sign == -1);
	while (arg[i])
	{
		if (acc > (limit - (unsigned long long)(arg[i] - '0')) / 10ULL)
			return (0);
		acc = (acc * 10ULL) + (unsigned long long)(arg[i++] - '0');
	}
	if (sign == -1 && acc == (unsigned long long)LLONG_MAX + 1ULL)
		*out = LLONG_MIN;
	else
		*out = (long long)acc * sign;
	return (1);
}

/**
 * Purpose: validate that `exit` argument is syntactically numeric.
 *
 * How it works:
 * 1. rejects `NULL` and empty strings.
 * 2. skips optional leading sign (`+` or `-`).
 * 3. rejects sign-only strings with no digits.
 * 4. verifies every remaining character is a decimal digit.
 * 5. returns 1 when valid, 0 otherwise.
 */
int	ft_isnum_exit_arg(char *arg)
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
 * Purpose: handle invalid numeric argument for `exit`.
 *
 * How it works:
 * 1. prints `minishell: exit: <arg>: numeric argument required`.
 * 2. frees shell resources before termination.
 * 3. exits with status code 2 (bash-compatible behavior).
 */
int	ft_exit_num_required(t_data *data, char *arg)
{
	ft_putstr_fd("minishell: exit: ", STDERR);
	ft_putstr_fd(arg, STDERR);
	ft_putstr_fd(": numeric argument required\n", STDERR);
	ft_free_shell(data);
	exit(2);
	return (0);
}

/**
 * Purpose: implement the `exit` builtin command behavior.
 *
 * How it works:
 * 1. validates input pointers and command vector.
 * 2. prints `exit` in interactive mode.
 * 3. exits with current status when no argument is provided.
 * 4. rejects extra arguments with status 1 and keeps shell running.
 * 5. validates/parses one numeric argument and exits with code modulo 256.
 */
int	ft_builtin_exit(t_data *data, t_cmd *cmd)
{
	long long	code;

	if (!data || !cmd || !cmd->argv || !cmd->argv[0])
		return (1);
	if (isatty(STDIN_FILENO))
		ft_putstr_fd("exit\n", STDERR);
	if (!cmd->argv[1])
	{
		ft_free_shell(data);
		exit(data->status);
		return (0);
	}
	if (cmd->argv[2])
		return (ft_putstr_fd("minishell: exit: too many arguments\n", STDERR),
			data->status = 1, 1);
	if (!ft_isnum_exit_arg(cmd->argv[1])
		|| !ft_parse_exit_ll(cmd->argv[1], &code))
		return (ft_exit_num_required(data, cmd->argv[1]));
	ft_free_shell(data);
	exit((unsigned char)code);
	return (0);
}
