/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_cd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/29 16:58:05 by vorhansa          #+#    #+#             */
/*   Updated: 2026/10/29 18:18:23 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/**
 * Behavior:
 * * cd with no arg → go to HOME
 * * cd PATH → chdir(PATH)
 * * update OLDPWD and PWD
 * 
 * OLDPWD and PWD are shell variables, so you should update your env list 
 * just like export.
 */
int	builtin_cd(t_data *data, t_cmd *cmd)
{
	char	*path;
	char	*oldpwd;
	char	*cwd;

	if (!cmd->argv[1])
		path = env_get(data->env, "HOME");
	else
		path = cmd->argv[1];
	if (!path)
		return (ERROR);
	oldpwd = getcwd(NULL, 0);
	if (chdir(path) == -1)
	{
		perror("cd");
		free(oldpwd);
		return (ERROR);
	}
	cwd = getcwd(NULL, 0);
	env_set(&data->env, "OLDPWD", oldpwd ? oldpwd : "");
	env_set(&data->env, "PWD", cwd);
	free(oldpwd);
	free(cwd);
	return (SUCCESS);
}
