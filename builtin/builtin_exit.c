/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_exit.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/29 16:58:46 by vorhansa          #+#    #+#             */
/*   Updated: 2026/10/29 17:52:51 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	builtin_exit(t_data *data, t_cmd *cmd)
{
	int	code;

	if (cmd->argv[1])
	{
		code = ft_atoi(cmd->argv[1]);
		if (code < 0)
			code = 256 + code;
	}
	else
		code = data->status;
	free_shell();
	exit(code % 256);
}
