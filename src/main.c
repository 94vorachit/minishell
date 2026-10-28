/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/26 15:53:51 by vorhansa          #+#    #+#             */
/*   Updated: 2026/10/28 01:21:11 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

t_data	*g_data;

int	update_shlvl(t_env *env, int shlvl)
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

int	init(t_data *mini, char **env)
{
	mini->status = 0; //This sets the shell's initial exit status to success.
	mini->cmd_count = 0; //No commands have been parsed yet.
	mini->tokens = NULL;
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
		ft_env_clear(&g_data->env);
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
        if (line[0] == '\0')
        {
        	free(line);
        	continue ;
        }
        if (lexer(line, &g_data->tokens) == 0)
        {
			// ft_print_tokens(g_data->tokens);
        	g_data->status = ft_validate_syntax(g_data->tokens);
        	if (g_data->status == SUCCESS)
			{
				if (expand_tokens(g_data, g_data->tokens) == -1)
					g_data->status = ERROR;
				else
					add_history(line);
			}
        }
        else
        	g_data->status = SYNTAX_ERROR;
        ft_token_clear(&g_data->tokens);
        free(line);
    }
	ft_env_clear(&g_data->env);
	free(g_data);
	return (SUCCESS);
}
