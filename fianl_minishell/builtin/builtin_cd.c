/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_cd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/20 14:43:38 by vorhansa          #+#    #+#             */
/*   Updated: 2026/11/20 16:56:31 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/**
 * Prints the standard shell-style error message for `cd` when a directory 
 * is invalid.
 */
void	ft_cd_error(char *arg)
{
	ft_putstr_fd("minishell: cd: ", STDERR);
	ft_putstr_fd(arg, STDERR);
	ft_putstr_fd(": No such file or directory\n", STDERR);
}

/**
 * Updates shell environment variables after a successful `cd`.
 * 
 * What it does:
 * 1. If there was a previous working directory, it stores it in `OLDPWD`
 * 2. Otherwise it stores an empty string
 * 3. Always stores the new working directory in `PWD`
 */
void	ft_cd_set_pwd(t_data *data, char *oldpwd, char *cwd)
{
	if (oldpwd)
		ft_env_set(&data->env, "OLDPWD", oldpwd);
	else
		ft_env_set(&data->env, "OLDPWD", "");
	ft_env_set(&data->env, "PWD", cwd);
}

/**
 * Resolves the directory path to use for `cd`
 * Handles validation for bad arguments
 * 
 * What it does:
 * 1. If no argument is given, it uses `HOME`
 * 2. Otherwise it uses the first argument
 * 3. If the path is missing, it returns ERROR
 * 4. If there are too many arguments (more than 1), it prints the error 
 * and returns `ERROR`
 */
int	ft_cd_get_path(t_data *data, t_cmd *cmd, char **path)
{
	if (!cmd || !cmd->argv)
		return (ERROR);
	if (cmd->argv[1] && cmd->argv[2])
	{
		ft_putstr_fd("minishell: cd: too many arguments\n", STDERR);
		return (ERROR);
	}
	if (!cmd->argv[1])
		*path = ft_env_get(data->env, "HOME");
	else
		*path = cmd->argv[1];
	if (!*path)
	{
		ft_putstr_fd("minishell: cd: HOME not set\n", STDERR);
		return (ERROR);
	}
	return (SUCCESS);
}

/**
 * Executes the actual directory change with `chdir()`
 * 
 * What it does:
 * 1. Saves the current directory in `oldpwd`
 * 2. Calls `chdir(path)`
 * 3. If it fails:
 * * prints the error
 * * frees the saved old path
 * * returns `ERROR`
 * 4. If it succeeds, returns `SUCCESS`
 * 
 * This is the actual system call step.
 */
int	ft_cd_change_dir(char *path, char **oldpwd)
{
	*oldpwd = getcwd(NULL, 0);
	if (chdir(path) == -1)
	{
		ft_cd_error(path);
		free(*oldpwd);
		*oldpwd = NULL;
		return (ERROR);
	}
	return (SUCCESS);
}

/**
 * Main `cd` builtin entry point
 * 
 * Flow:
 * 1. Resolve the target path with `ft_cd_get_path()`
 * 2. Attempt the actual directory change with `ft_cd_change_dir()`
 * 3. Get the new current directory with `getcwd()`
 * 4. Update `OLDPWD` and `PWD`
 * 5. Free memory and return success
 * 
 * Return behavior:
 * * `SUCCESS = 0` on proper `cd`
 * * `ERROR = 1` on invalid path or bad arguments
 */
int	ft_builtin_cd(t_data *data, t_cmd *cmd)
{
	char	*path;
	char	*oldpwd;
	char	*cwd;

	if (ft_cd_get_path(data, cmd, &path) == ERROR)
		return (ERROR);
	if (ft_cd_change_dir(path, &oldpwd) == ERROR)
		return (ERROR);
	cwd = getcwd(NULL, 0);
	ft_cd_set_pwd(data, oldpwd, cwd);
	free(oldpwd);
	free(cwd);
	return (SUCCESS);
}
