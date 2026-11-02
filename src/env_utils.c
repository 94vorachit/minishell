/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/29 16:40:35 by vorhansa          #+#    #+#             */
/*   Updated: 2026/11/02 17:12:20 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/**
 * Updates an existing env variable value.
 * 
 * Takes an existing env node, a name, and a new value.
 * Builds a full "`NAME=value`" string, frees the old value, and replaces it.
 * Used when a variable already exists and is being updated.
 */
int	update_env_value(t_env *node, char *name, char *value)
{
	char	*tmp;
	char	*new_value;

	tmp = ft_strjoin(name, "=");
	if (!tmp)
		return (-1);
	new_value = ft_strjoin(tmp, value);
	free(tmp);
	if (!new_value)
		return (-1);
	free(node->value);
	node->value = new_value;
	return (0);
}

/**
 * Add node to the end of the environment list.
 * 
 * If the list is empty, it becomes the first node.
 * This is important because new variables should be appended, 
 * not inserted at the head.
 */
int	env_add_last(t_env **env, t_env *node)
{
	t_env	*last;

	if (!*env)
	{
		*env = node;
		return (0);
	}
	last = *env;
	while (last->next)
		last = last->next;
	last->next = node;
	return (0);
}

/**
 * Main function to set or create an env variable.
 * 
 * It first checks whether the name already exists:
 * * if yes → update the value
 * * if no → create a new node and append it to the end
 * It builds "NAME=value" internally before storing in the list.
 */
int	env_set(t_env **env, char *name, char *value)
{
	t_env	*node;
	char	*tmp;
	char	*new_value;

	if (!env || !name)
		return (-1);
	node = env_find(*env, name);
	if (node)
		return (update_env_value(node, name, value));
	tmp = ft_strjoin(name, "=");
	if (!tmp)
		return (-1);
	new_value = ft_strjoin(tmp, value);
	free(tmp);
	if (!new_value)
		return (-1);
	node = malloc(sizeof(*node));
	if (!node)
	{
		free(new_value);
		return (-1);
	}
	node->value = new_value;
	node->next = NULL;
	return (env_add_last(env, node));
}

/**
 * Removes the matching variable from the linked list
 * 
 * Removes one variable from the env list by matching its name.
 * It walks the list and compares the beginning
 * of each value with the target name.
 * When it finds "NAME=", it removes that node and frees its memory.
 */
void	env_unset(t_env **env, char *name)
{
	t_env	*cur;
	t_env	*prev;

	if (!env || !*env || !name)
		return ;
	prev = NULL;
	cur = *env;
	while (cur)
	{
		if (ft_strncmp(cur->value, name, ft_strlen(name)) == 0
			&& cur->value[ft_strlen(name)] == '=')
		{
			if (prev)
				prev->next = cur->next;
			else
				*env = cur->next;
			free(cur->value);
			free(cur);
			return ;
		}
		prev = cur;
		cur = cur->next;
	}
}

/**
 * Counts total env entries
 */
int	env_count(t_env *env)
{
	int		count;

	count = 0;
	while (env)
	{
		count++;
		env = env->next;
	}
	return (count);
}
