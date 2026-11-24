/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export_print.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/20 19:05:00 by vorhansa          #+#    #+#             */
/*   Updated: 2026/11/20 18:59:50 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/**
 * Purpose: count entries in a NULL-terminated string array.
 *
 * How it works:
 * 1. starts an index at zero
 * 2. increments until it reaches the NULL terminator
 * 3. returns the number of stored entries
 */
int	ft_array_count(char **array)
{
	int	count;

	count = 0;
	while (array && array[count])
		count++;
	return (count);
}

/**
 * Purpose: merge environment and export-only entries for sorted display.
 *
 * How it works:
 * 1. counts entries in both lists
 * 2. allocates one combined array
 * 3. copies environment entries, then export-only entries
 * 4. adds a NULL terminator and returns the combined array
 */
char	**ft_merge_export_lists(char **env_list, char **exp_list)
{
	char	**comb;
	int		env_count;
	int		exp_count;
	int		i;

	env_count = ft_array_count(env_list);
	exp_count = ft_array_count(exp_list);
	comb = malloc(sizeof(char *) * (env_count + exp_count + 1));
	if (!comb)
		return (NULL);
	i = 0;
	env_count = 0;
	while (env_list && env_list[env_count])
		comb[i++] = env_list[env_count++];
	exp_count = 0;
	while (exp_list && exp_list[exp_count])
		comb[i++] = exp_list[exp_count++];
	comb[i] = NULL;
	return (comb);
}

/**
 * Purpose: print all exported variables in alphabetical Bash-style output.
 *
 * How it works:
 * 1. creates sorted copies of the environment and export-only lists
 * 2. merges both lists into one array
 * 3. sorts the merged array alphabetically
 * 4. prints each entry and frees temporary arrays
 */
void	ft_print_sorted_export(t_data *data)
{
	char	**env_list;
	char	**exp_list;
	char	**comb;
	int		i;

	env_list = ft_sort_env_alpha(data->env);
	exp_list = ft_sort_env_alpha(data->export);
	comb = ft_merge_export_lists(env_list, exp_list);
	if (!comb)
		return ;
	i = ft_array_count(comb);
	ft_sort_env_array(comb, i);
	i = 0;
	while (comb[i])
		ft_export_print_entry(comb[i++]);
	free(comb);
	free(env_list);
	free(exp_list);
}
