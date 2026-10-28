/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_path.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/29 02:12:56 by vorhansa          #+#    #+#             */
/*   Updated: 2026/10/29 03:25:19 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/**
 * Checks whether a command exists and is executable inside one directory.
 * 
 * For example:
 * `dir = "/usr/bin";`
 * `cmd = "ls";`
 * 
 * It builds:
 * `/usr/bin/ls`
 * 
 * 1. Add / to the directory
 * 2. Add the command name
 * 3. Check execute permission
 * 
 * If the check succeeds, the function returns the allocated path:
 * `"/usr/bin/ls"`
 */
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

/**
 * Searches for a command in all directories from PATH.
 * 
 * For example, after splitting:
 * `PATH=/usr/local/bin:/usr/bin:/bin`
 * 
 * the array is:
 * paths[0] = "/usr/local/bin"
 * paths[1] = "/usr/bin"
 * paths[2] = "/bin"
 * paths[3] = NULL
 * 
 * The function loops over every directory:
 * 
 * For the command `ls`, it checks:
 * /usr/local/bin/ls
 * /usr/bin/ls
 * /bin/ls
 * 
 * It stops at the first executable path found
 * If no directory contains an executable command, it returns: `NULL`
 */
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

/**
 * This is the main public function in the file. It receives:
 * * `env`: the shell environment linked list
 * * `cmd`: the command name, such as `"ls"` or `"cat"`
 * 
 * It returns:
 * * A dynamically allocated executable path on success
 * * NULL if the command cannot be found
 * 
 * 1. Validate the command : reject NULL and ""
 * 2. Handle commands containing / Examples:
 * /bin/ls
 * ./minishell
 * ../program
 * 3. Get PATH : /usr/local/bin:/usr/bin:/bin
 * 4. Split PATH to:
 * paths[0] = "/usr/local/bin"
 * paths[1] = "/usr/bin"
 * paths[2] = "/bin"
 * paths[3] = NULL
 * 5. Search all directories
 * 6. Free the split array
 * 7. Return the result
 */
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
