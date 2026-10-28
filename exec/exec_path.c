/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_path.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/29 02:12:56 by vorhansa          #+#    #+#             */
/*   Updated: 2026/10/29 02:13:26 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

char	*check_executable_in_dir(const char *dir, const char *cmd)
{
	char	*full_path;
	char	*full_cmd;

	full_path = ft_strjoin(dir, "/");
	if (!full_path)
		return (NULL);
	full_cmd = ft_strjoin(full_path, cmd);
	free(full_path);
	if (!full_cmd)
		return (NULL);
	if (access(full_cmd, X_OK) == 0)
		return (full_cmd);
	free(full_cmd);
	return (NULL);
}

char	*search_in_paths(char **paths, const char *cmd)
{
	int		i;
	char	*result;

	i = 0;
	while (paths[i])
	{
		result = check_executable_in_dir(paths[i], cmd);
		if (result)
			return (result);
		i++;
	}
	return (NULL);
}

char	*find_executable_path(t_env *env, const char *cmd)
{
	char	*path_env;
	char	**paths;
	char	*result;

	if (!cmd || cmd[0] == '\0')
		return (NULL);
	if (ft_strchr(cmd, '/'))
		return (ft_strdup(cmd));
	path_env = env_get(env, "PATH");
	if (!path_env || !path_env[0])
		return (NULL);
	paths = ft_split(path_env, ':');
	if (!paths)
		return (NULL);
	result = search_in_paths(paths, cmd);
	ft_free_split(paths);
	return (result);
}
