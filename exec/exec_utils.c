/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/29 01:46:38 by vorhansa          #+#    #+#             */
/*   Updated: 2026/11/09 17:56:40 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/**
 * Purpose: free all memory inside one `t_cmd`.
 * 
 * How it works:
 * 1. frees each `argv` string
 * 2. frees `argv`
 * 3. frees `infile`, `outfile`, `heredoc`
 * 4. closes heredoc fd if open
 * 5. resets members to safe defaults
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
	free(cmd->heredoc);
	if (cmd->heredoc_fd != -1)
		close (cmd->heredoc_fd);
	cmd->infile = NULL;
	cmd->outfile = NULL;
	cmd->heredoc = NULL;
	cmd->heredoc_fd = -1;
	cmd->append = 0;
}

/**
 * Purpose: free a whole linked list of commands.
 * 
 * How it works:
 * 1. iterate over list
 * 2. call `ft_free_cmd` on each node
 * 3. free the node itself
 */
void	ft_free_cmd_list(t_cmd **cmds)
{
	t_cmd	*cur;
	t_cmd	*next;

	if (!cmds)
		return ;
	cur = *cmds;
	while (cur)
	{
		next = cur->next;
		ft_free_cmd(cur);
		free(cur);
		cur = next;
	}
	*cmds = NULL;
}

/** 
 * Purpose: count environment variables.
 * 
 * How it works:
 * 1. walks linked list
 * 2. increments count until `NULL`
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
 * Purpose: copy environment linked list into array.
 * 
 * How it works:
 * 1. duplicates each `env->value`
 * 2. stores into `char **arr`
 * 3. appends terminating `NULL`
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
 * Purpose: convert env linked list to `execve()` format.
 * 
 * `execve()` expects: `char *const envp[]`
 * 
 * How it works:
 * 1. allocate array with size `count + 1`
 * 2. fill with environment strings
 * 3. return `NULL`-terminated array
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
