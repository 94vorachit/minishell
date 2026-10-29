/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_utils2.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/29 16:52:39 by vorhansa          #+#    #+#             */
/*   Updated: 2026/10/29 16:53:32 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/**
 * Searches an env node by the name before =
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
