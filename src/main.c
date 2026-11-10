/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/26 15:53:51 by vorhansa          #+#    #+#             */
/*   Updated: 2026/11/10 17:21:55 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	ft_error(int error_code)
{
	(void)error_code;
	ft_putstr_fd("minishell: memory allocation failed\n", STDERR);
	exit(ERROR);
}

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
	// char	*cwd;

	mini->status = 0; //This sets the shell's initial exit status to success.
	mini->tokens = NULL;
	mini->export = NULL;
	if (ft_init_env(&mini->env, env) == -1)
		return (-1);
	ft_shlvl(&mini->env);
	return (0);
}

int	main(int ac, char **av, char **env)
{
	char	*line;
	t_data	*data;
	t_cmd	cmd;

	(void)av;
	if (ac != 1)
	{
		ft_putstr_fd("minishell: Use ./minishell without arguments", 2);
		return (0);
	}
	data = malloc(sizeof(*data));
	if (!data)
		return (ERROR);
	if (init(data, env) == -1)
	{
		ft_env_clear(&data->env);
		ft_env_clear(&data->export);
		free(data);
		return (ERROR);
	}
	sig_init();
	while (1)
	{
		line = readline("minishell > ");
		if (g_signal == SIGINT || g_signal == SIGQUIT)
		{
			data->status = 128 + g_signal;
			g_signal = 0;
		}
		if (!line)
		{
			ft_putstr_fd("exit\n", STDERR);
			free_shell(data);
			return (data->status);
		}
		if (line[0] == '\0')
		{
			free(line);
			continue ;
		}
		add_history(line);
		if (lexer(line, &data->tokens) == 0)
		{
			if (ft_validate_syntax(data->tokens) == SUCCESS)
			{
				if (expand_tokens(data, &data->tokens) == -1)
					data->status = ERROR;
				else if (ft_has_pipe(data->tokens))
				{
					t_cmd	*cmds;
					int	build_ret;

					cmds = NULL;
					build_ret = build_pipeline_cmds(data->tokens, &cmds);
					if (build_ret == 0)
						data->status = execute_pipeline(data, cmds);
					else if (build_ret == ERROR)
						data->status = ERROR;
					else
						data->status = SYNTAX_ERROR;
					ft_free_cmd_list(&cmds);
				}
				else
				{
					int	build_ret;

					build_ret = build_simple_cmd(data->tokens, &cmd);
					if (build_ret == 0)
						data->status = execute_simple_cmd(data, &cmd);
					else if (build_ret == ERROR)
						data->status = ERROR;
					else
						data->status = SYNTAX_ERROR;
					ft_free_cmd(&cmd);
				}
			}
			else
				data->status = SYNTAX_ERROR;
		}
		else
			data->status = SYNTAX_ERROR;
		ft_token_clear(&data->tokens);
		free(line);
	}
	free_shell(data);
	return (data->status);
}
