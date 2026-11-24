/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export_utils.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/20 18:48:51 by vorhansa          #+#    #+#             */
/*   Updated: 2026/11/20 18:59:50 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/**
 * Checks whether an environment variable name is valid for `export`.
 * 
 * What it does:
 * 1. Rejects `NULL` or empty strings
 * 2. First character must be a letter or `_`
 * 3. Remaining characters must be letters, digits, or `_`
 */
int	ft_export_valid_name(char *name)
{
	int	i;

	if (!name || !*name)
		return (0);
	if (!(ft_isalpha((unsigned char)name[0]) || name[0] == '_'))
		return (0);
	i = 1;
	while (name[i])
	{
		if (!(ft_isalnum((unsigned char)name[i]) || name[i] == '_'))
			return (0);
		i++;
	}
	return (1);
}

/**
 * Shared validation error handler for invalid export names.
 */
int	ft_export_invalid_name(char *name, char *equal)
{
	if (!ft_export_valid_name(name))
	{
		ft_putstr_fd("minishell: export: `", STDERR);
		ft_putstr_fd(name, STDERR);
		if (equal)
		{
			ft_putstr_fd("=", STDERR);
			ft_putstr_fd(equal + 1, STDERR);
		}
		ft_putstr_fd("': not a valid identifier\n", STDERR);
		return (ERROR);
	}
	return (SUCCESS);
}

/**
 * Purpose: find an exact variable name in the export-only list.
 *
 * How it works:
 * 1. walks through every export-only environment node
 * 2. compares the complete stored name with the requested name
 * 3. returns 1 when found and 0 otherwise
 */
int	ft_export_find_name(t_env *export, char *name)
{
	while (export)
	{
		if (ft_strcmp(export->value, name) == 0)
			return (1);
		export = export->next;
	}
	return (0);
}

/**
 * Purpose: print one entry in Bash-style declare format.
 *
 * How it works:
 * 1. searches for '=' in the entry
 * 2. prints 'declare -x NAME' for export-only variables
 * 3. prints 'declare -x NAME="value"' for environment variables
 */
void	ft_export_print_entry(char *entry)
{
	char	*equal;

	equal = ft_strchr(entry, '=');
	if (!equal)
	{
		printf("declare -x %s\n", entry);
		return ;
	}
	printf("declare -x ");
	while (entry < equal)
	{
		putchar(*entry);
		entry++;
	}
	printf("=\"%s\"\n", equal + 1);
}
