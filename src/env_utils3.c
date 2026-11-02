/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_utils3.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/02 16:39:45 by vorhansa          #+#    #+#             */
/*   Updated: 2026/11/02 17:24:36 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/**
 * Creates a temporary array of strings from the environment list.
 * 
 * * It walks through the linked-list of env variables and 
 * copies each valid value into a char ** array.
 * * It also sets the last element to NULL, so the array is safe 
 * to use like a C string array.
 * 
 * This is just a temporary working copy; it does not modify 
 * the linked list itself.
 */
char	**copy_env_array(t_env *env, int count)
{
	char	**arr;
	t_env	*cur;
	int		i;

	arr = malloc(sizeof(char *) * (count + 1));
	if (!arr)
		return (NULL);
	i = 0;
	cur = env;
	while (cur)
	{
		if (cur->value)
			arr[i++] = cur->value;
		cur = cur->next;
	}
	arr[i] = NULL;
	return (arr);
}

/**
 * Sorts the temporary string array alphabetically.
 * 
 * It uses a simple bubble-like selection logic:
 * *  compare two entries with ft_strcmp
 * * swap them if the left is greater than the right
 * 
 * This keeps the environment list unchanged while ordering 
 * the printed export output.
 */
void	sort_env_array(char **arr, int count)
{
	int		i;
	int		j;
	char	*tmp;

	if (count < 2)
		return ;
	i = 0;
	while (i < count)
	{
		j = i + 1;
		while (j < count)
		{
			if (ft_strcmp(arr[i], arr[j]) > 0)
			{
				tmp = arr[i];
				arr[i] = arr[j];
				arr[j] = tmp;
			}
			j++;
		}
		i++;
	}
}

/**
 * Main public function in this file.
 * 
 * * Counts how many env entries exist, creates the temporary array, 
 * and then sorts it.
 * * Returns the sorted string array for printing by the export builtin.
 * 
 * It is not the real env storage; it is just a sorted view used for display.
 */
char	**sort_env_alpha(t_env *env)
{
	char	**arr;
	int		count;

	count = env_count(env);
	arr = copy_env_array(env, count);
	if (!arr)
		return (NULL);
	sort_env_array(arr, count);
	return (arr);
}
