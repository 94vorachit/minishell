/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/10 18:01:29 by vorhansa          #+#    #+#             */
/*   Updated: 2026/11/10 18:37:51 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/**
 * Purpose: print a shell-level memory failure and terminate execution.
 *
 * How it works:
 * 1. accepts an error code but ignores it for this project
 * 2. prints a clear minishell error to stderr
 * 3. exits with the shell's generic error code
 *
 * Return: none; this function exits the program.
 */
void	ft_error(int error_code)
{
	(void)error_code;
	ft_putstr_fd("minishell: memory allocation failed\n", STDERR);
	exit(ERROR);
}

/**
 * Purpose: free the shell's runtime state before exit.
 *
 * How it works:
 * 1. clears readline history
 * 2. frees token list, environment list, and export list
 * 3. frees the main shell data structure itself
 *
 * Return: none.
 */
void	free_shell(t_data *data)
{
	rl_clear_history();
	if (data)
	{
		ft_token_clear(&data->tokens);
		ft_env_clear(&data->env);
		ft_env_clear(&data->export);
		free(data);
	}
}

/**
 * Purpose: update the shell-level SHLVL variable.
 *
 * How it works:
 * 1. read the current SHLVL from the environment
 * 2. increment it by one
 * 3. convert it back to a string
 * 4. store it back in the env list
 *
 * Return: none.
 */
void	ft_shlvl(t_env **env)
{
	char	*val;
	char	*level;
	int		shlvl;

	val = getenv("SHLVL");
	if (!val || val[0] == '\0')
		return ;
	shlvl = ft_atoi(val) + 1;
	level = ft_itoa(shlvl);
	if (!level)
		return ;
	env_set(env, "SHLVL", level);
	free(level);
}

/**
 * Purpose: build the initial environment linked list from the parent shell.
 *
 * How it works:
 * 1. walk the raw env array
 * 2. create one `t_env` node per entry
 * 3. append each node to the list
 * 4. return failure if any allocation fails
 *
 * Return: 0 on success, -1 on allocation failure.
 */
int	ft_init_env(t_env **mini_env, char **env)
{
	t_env	*new;
	t_env	*last;
	int		i;

	*mini_env = NULL;
	last = NULL;
	i = 0;
	while (env && env[i])
	{
		new = ft_env_new(env[i++]);
		if (!new)
		{
			ft_env_clear(mini_env);
			return (-1);
		}
		if (!*mini_env)
			*mini_env = new;
		else
			last->next = new;
		last = new;
	}
	return (0);
}

/**
 * Purpose: initialize the main shell data struct.
 *
 * How it works:
 * 1. reset the shell status
 * 2. clear token and export lists
 * 3. copy parent environment into `mini->env`
 * 4. update SHLVL for the minishell session
 *
 * Return: 0 on success, -1 on env initialization failure.
 */
int	init(t_data *mini, char **env)
{
	mini->status = 0;
	mini->tokens = NULL;
	mini->export = NULL;
	if (ft_init_env(&mini->env, env) == -1)
		return (-1);
	ft_shlvl(&mini->env);
	return (0);
}
