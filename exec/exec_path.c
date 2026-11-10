/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_path.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/29 02:12:56 by vorhansa          #+#    #+#             */
/*   Updated: 2026/11/10 15:24:00 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/**
 * Purpose: free a NULL-terminated array of strings.
 * 
 * How it works:
 * 1. loops through each string
 * 2. frees each element
 * 3. frees the array itself
 */
void	ft_free_split(char **arr)
{
	int	i;

	if (!arr)
		return ;
	i = 0;
	while (arr[i])
	{
		free(arr[i]);
		i++;
	}
	free(arr);
}

/**
 * Purpose: test whether command exists in one directory.
 * 
 * For example:
 * `dir = "/usr/bin";`
 * `cmd = "ls";`
 * 
 * It builds:
 * `/usr/bin/ls`
 * 
 * How it works:
 * 1. forms `dir + "/" + cmd`
 * 2. checks `access(path, X_OK)`
 * 3. returns allocated path if executable
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
 * Purpose: search every PATH directory.
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
 * How it works:
 * 1. loops over `PATH` entries split by `:`
 * 2. calls `check_executable_in_dir`
 * 3. stops at first valid match

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
 * Purpose: find the executable file for a command name.
 * 
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
 * 5. Search all directories with `search_in_paths`
 * 6. Free the split array
 * 7. return full path or `NULL`
 */
char	*find_executable_path(t_env *env, const char *cmd)
{
	char	*path_env;
	char	**paths;
	char	*result;

	if (!cmd || cmd[0] == '\0')
		return (NULL);
	if (ft_strchr(cmd, '/'))
	{
		if (access(cmd, F_OK) != 0)
			return (NULL);
		return (ft_strdup(cmd));
	}
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
