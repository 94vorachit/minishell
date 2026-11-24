/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_echo.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/20 14:43:47 by vorhansa          #+#    #+#             */
/*   Updated: 2026/11/20 14:43:49 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/**
 * print arguments separated by spaces
 * add newline unless -n
 * no fancy Bash flags beyond -n
 */
int	ft_builtin_echo(t_data *data, t_cmd *cmd)
{
	int	i;
	int	newline;

	(void)data;
	i = 1;
	newline = 1;
	while (cmd->argv[i] && ft_strcmp(cmd->argv[i], "-n") == 0)
	{
		newline = 0;
		i++;
	}
	while (cmd->argv[i])
	{
		ft_putstr_fd(cmd->argv[i], STDOUT);
		if (cmd->argv[i + 1])
			ft_putstr_fd(" ", STDOUT);
		i++;
	}
	if (newline)
		ft_putstr_fd("\n", STDOUT);
	return (SUCCESS);
}
