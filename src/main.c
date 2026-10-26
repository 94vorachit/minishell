/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/26 15:53:51 by vorhansa          #+#    #+#             */
/*   Updated: 2026/10/26 20:21:44 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

t_data	*g_data;

static int	update_shlvl(t_env *env, int shlvl)
{
	char	*level;
	char	*new_value;

	level = ft_itoa(shlvl);
	if (!level)
		return (0);
	new_value = ft_strjoin("SHLVL=", level);
	free(level);
	if (!new_value)
		return (0);
	free(env->value);
	env->value = new_value;
	return (1);
}

void	ft_shlvl(t_env *env)
{
	char	*val;
	int		shlvl;

	val = getenv("SHLVL");
	if (!val || val[0] == '\0')
		return ;
	shlvl = ft_atoi(val) + 1;
	while (env)
	{
		if (ft_strncmp("SHLVL=", env->value, 6) == 0
			&& update_shlvl(env, shlvl))
			return ;
		env = env->next;
	}
}

t_env *ft_lstadd_val(char *value, t_env *next)
{
	t_env *new;

	new = malloc(sizeof(t_env));
	if (!new)
		return (NULL);
	new->value = value;
	new->next = next;

	return new;
}

static int ft_init_env(t_env **mini_env, char **env)
{
	t_env	*temp;
	int		i;

	*mini_env = NULL;
	if (!env || !env[0])
		return (0);
	i = 0;
	temp = ft_lstadd_val(ft_strdup(env[i++]), NULL);
	if (!temp)
		return (-1);
	*mini_env = temp;
	while (env[i])
	{
		temp->next = ft_lstadd_val(ft_strdup(env[i++]), NULL);
		if (!temp->next)
			return (-1);
		temp = temp->next;
	}
	return (0);	
}

int	init(t_data *mini, char **env)
{
	mini->status = 0; //This sets the shell's initial exit status to success.
	mini->cmd_count = 0; //No commands have been parsed yet.
	if (ft_init_env(&mini->env, env) == -1)
		return (-1);
	ft_shlvl(mini->env);
	return (0);
}

int main(int ac, char **av, char **env)
{
    char	*line;
    
    (void) av;
	if (ac != 1)
	{
		// puterror("\e[0;37mUse", "./minishell without arguments");
        ft_putstr_fd("minishell: Use ./minishell without arguments", 2);
		return (0);
	}
    g_data = malloc(sizeof(*g_data));
    if (!g_data)
        return (ERROR);
    if (init(g_data, env) == -1)
    {
        free(g_data);
        return (ERROR);
    }
    sig_init();
    while (1)
    {
        line = readline("minishell > ");
        if (!line)
		{
			ft_putstr_fd("exit\n", STDERR);
			return (SUCCESS);
		}
    }
    return (0);
}
