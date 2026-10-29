/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/29 16:40:35 by vorhansa          #+#    #+#             */
/*   Updated: 2026/10/29 16:53:05 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/**
 * Builds the full "NAME=value" string.
 */
char	*build_env_string(char *name, char *value)
{
	char	*tmp;
	char	*result;

	tmp = ft_strjoin(name, "=");
	if (!tmp)
		return (NULL);
	result = ft_strjoin(tmp, value);
	free(tmp);
	return (result);
}

/**
 * Updates an existing env variable value.
 */
int	update_env_value(t_env *node, char *name, char *value)
{
	char	*new_value;

	new_value = build_env_string(name, value);
	if (!new_value)
		return (-1);
	free(node->value);
	node->value = new_value;
	return (0);
}

/**
 * Updates existing value or adds a new node
 */
int	env_set(t_env **env, char *name, char *value)
{
	t_env	*node;
	char	*new_value;

	if (!env || !name)
		return (-1);
	node = env_find(*env, name);
	if (node)
		return (update_env_value(node, name, value));
	new_value = build_env_string(name, value);
	if (!new_value)
		return (-1);
	node = malloc(sizeof(*node));
	if (!node)
	{
		free(new_value);
		return (-1);
	}
	node->value = new_value;
	node->next = *env;
	*env = node;
	return (0);
}

/**
 * Removes the matching variable from the linked list
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
