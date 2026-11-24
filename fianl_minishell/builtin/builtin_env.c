/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_env.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/20 14:43:53 by vorhansa          #+#    #+#             */
/*   Updated: 2026/11/20 20:55:18 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/**
 * Purpose: print the shell environment, matching Bash's basic `env` output.
 *
 * How it works:
 * 1. ignore command arguments for this builtin implementation
 * 2. walk the real environment linked list
 * 3. print entries containing '=' as `NAME=value`
 * 4. skip export-only variables because they are not in the environment
 *
 * Return: SUCCESS (0).
 */
int	ft_builtin_env(t_data *data, t_cmd *cmd)
{
	t_env	*cur;

	(void)cmd;
	ft_env_set(&data->env, "_", "/usr/bin/env");
	cur = data->env;
	while (cur)
	{
		if (cur->value && ft_strchr(cur->value, '='))
		{
			ft_putstr_fd(cur->value, STDOUT);
			ft_putstr_fd("\n", STDOUT);
		}
		cur = cur->next;
	}
	return (SUCCESS);
}
