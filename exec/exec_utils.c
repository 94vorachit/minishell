/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/29 01:46:38 by vorhansa          #+#    #+#             */
/*   Updated: 2026/10/29 02:52:10 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/**
 * Frees a dynamically allocated array of strings.
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
 * Releases all dynamically allocated memory owned by a command.
 * 
 * Frees argv : For example:
 * argv
 ├── "echo"
 ├── "hello"
 └── NULL
 * Then it sets cmd->argv to NULL to prevent it from pointing to freed memory.
 * 
 * Frees redirection filenames
 * cmd->infile
 * cmd->outfile
 * 
 * This allows the same t_cmd structure to be reused safely.
*/
void	ft_free_cmd(t_cmd *cmd)
{
	int	i;

	if (!cmd)
		return ;
	if (cmd->argv)
	{
		i = 0;
		while (cmd->argv[i])
		{
			free(cmd->argv[i]);
			i++;
		}
		free(cmd->argv);
		cmd->argv = NULL;
	}
	free(cmd->infile);
	free(cmd->outfile);
	cmd->infile = NULL;
	cmd->outfile = NULL;
	cmd->append = 0;
}

/** 
 * Counts the number of nodes in the shell environment linked list.
 * 
 * The environment is stored as a linked list:
 * env
 ├── "USER=alice"
 ├── "PATH=/usr/bin:/bin"
 ├── "HOME=/home/alice"
 └── NULL
 * 
 * For three environment nodes, it returns: 3
 */
int	ft_count_env(t_env *env)
{
	int	count;

	count = 0;
	while (env)
	{
		count++;
		env = env->next;
	}
	return (count);
}

/**
 * Copies the environment linked list into a NULL-terminated 
 * array of strings.
 * 
 * If fails:
 * 1. Frees all previously copied strings.
 * 2. Frees the array.
 * 3. Returns `-1`.
 * 
 * On success, it returns`0`.
 */
int	ft_copy_env(char **arr, t_env *env)
{
	int	i;

	i = 0;
	while (env)
	{
		arr[i] = ft_strdup(env->value);
		if (!arr[i])
		{
			ft_free_split(arr);
			return (-1);
		}
		i++;
		arr[i] = NULL;
		env = env->next;
	}
	return (0);
}

/**
 * Converts the shell's linked-list environment into the format 
 * required by `execve()`.
 * 
 * `execve()` expects: `char *const envp[]`
 * 
 * which is effectively a NULL-terminated char ** array:
 * 
 * envp
 ├── "USER=alice"
 ├── "PATH=/usr/bin:/bin"
 ├── "HOME=/home/alice"
 └── NULL
 *
 * On success, it returns the completed array.
 * 
 * It is used before executing a program:
 * `envp = env_to_char_array(data->env);`
 * `execve(exec_path, cmd->argv, envp);`
 */
char	**env_to_char_array(t_env *env)
{
	char	**arr;

	arr = malloc(sizeof(char *) * (ft_count_env(env) + 1));
	if (!arr)
		return (NULL);
	arr[0] = NULL;
	if (ft_copy_env(arr, env) == -1)
		return (NULL);
	return (arr);
}
