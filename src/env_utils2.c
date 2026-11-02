/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_utils2.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/29 16:52:39 by vorhansa          #+#    #+#             */
/*   Updated: 2026/11/02 17:16:18 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/**
 * Searches the environment linked list for a variable whose 
 * name matches the given string.
 * 
 * * It compares the start of each stored value with the target name 
 * and checks that the next character is '='.
 * * Returns the matching node, or NULL if not found.
 * * This is used before updating or replacing an existing variable.
 */
t_env	*env_find(t_env *env, char *name)
{
	t_env	*cur;

	if (!env || !name)
		return (NULL);
	cur = env;
	while (cur)
	{
		if (ft_strncmp(cur->value, name, ft_strlen(name)) == 0
			&& cur->value[ft_strlen(name)] == '=')
			return (cur);
		cur = cur->next;
	}
	return (NULL);
}

/**
 * Finds the value of a variable by name.
 * 
 * * It walks the list and compares each stored entry against 
 * the requested name.
 * * When it matches, it returns the pointer just after '='.
 * 
 * Example: if the node is "PATH=/bin", then it returns "/bin".
 */
char	*env_get(t_env *env, const char *name)
{
	size_t	name_len;

	if (!name || name[0] == '\0')
		return (NULL);
	name_len = ft_strlen(name);
	while (env)
	{
		if (ft_strncmp(env->value, name, name_len) == 0
			&& env->value[name_len] == '=')
			return (env->value + name_len + 1);
		env = env->next;
	}
	return (NULL);
}

/**
 * Frees the whole environment linked list.
 * 
 * * It loops through every node, frees each value string, 
 * frees the node itself, and then sets the list head to NULL.
 * * Used when shutting down the shell or freeing memory on failure.
 */
void	ft_env_clear(t_env **env)
{
	t_env	*current;
	t_env	*next;

	current = *env;
	while (current)
	{
		next = current->next;
		free(current->value);
		free(current);
		current = next;
	}
	*env = NULL;
}

/**
 * Creates a new environment node.
 * 
 * * Allocates the node, duplicates the input string into node->value, 
 * and initializes next to NULL.
 * * Returns NULL on allocation failure.
 */
t_env	*ft_env_new(char *value)
{
	t_env	*new;

	new = malloc(sizeof(*new));
	if (!new)
		return (NULL);
	new->value = ft_strdup(value);
	if (!new->value)
	{
		free(new);
		return (NULL);
	}
	new->next = NULL;
	return (new);
}
