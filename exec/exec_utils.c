/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/29 01:46:38 by vorhansa          #+#    #+#             */
/*   Updated: 2026/10/29 02:02:12 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

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
