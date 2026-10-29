/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_pwd.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/29 16:58:55 by vorhansa          #+#    #+#             */
/*   Updated: 2026/10/29 17:52:30 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	builtin_pwd(t_data *data, t_cmd *cmd)
{
	char	*cwd;

	(void)data;
	(void)cmd;
	cwd = getcwd(NULL, 0);
	if (!cwd)
		return (ERROR);
	ft_putstr_fd(cwd, STDOUT);
	ft_putstr_fd("\n", STDOUT);
	free(cwd);
	return (SUCCESS);
}
