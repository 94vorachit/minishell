/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_cd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/29 16:58:05 by vorhansa          #+#    #+#             */
/*   Updated: 2026/11/02 14:48:13 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	cd_error(char *arg)
{
	ft_putstr_fd("cd : no such file or directory : ", STDERR);
	ft_putstr_fd(arg, STDERR);
	ft_putstr_fd("\n", STDERR);
}

void	cd_set_pwd(t_data *data, char *oldpwd, char *cwd)
{
	if (oldpwd)
		env_set(&data->env, "OLDPWD", oldpwd);
	else
		env_set(&data->env, "OLDPWD", "");
	env_set(&data->env, "PWD", cwd);
}

int	cd_get_path(t_data *data, t_cmd *cmd, char **path)
{
	if (!cmd || !cmd->argv)
		return (ERROR);
	if (!cmd->argv[1])
		*path = env_get(data->env, "HOME");
	else
		*path = cmd->argv[1];
	if (!*path)
		return (ERROR);
	if (cmd->argv[2])
	{
		cd_error(cmd->argv[2]);
		return (ERROR);
	}
	return (SUCCESS);
}

int	cd_change_dir(char *path, char **oldpwd)
{
	*oldpwd = getcwd(NULL, 0);
	if (chdir(path) == -1)
	{
		cd_error(path);
		free(*oldpwd);
		*oldpwd = NULL;
		return (ERROR);
	}
	return (SUCCESS);
}

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

	if (cd_get_path(data, cmd, &path) == ERROR)
		return (ERROR);
	if (cd_change_dir(path, &oldpwd) == ERROR)
		return (ERROR);
	cwd = getcwd(NULL, 0);
	cd_set_pwd(data, oldpwd, cwd);
	free(oldpwd);
	free(cwd);
	return (SUCCESS);
}
