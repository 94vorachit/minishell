/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/28 19:36:16 by vorhansa          #+#    #+#             */
/*   Updated: 2026/10/29 02:19:24 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	count_args(t_token *tokens)
{
	t_token	*cur;
	int		argc;

	argc = 0;
	cur = tokens;
	while (cur && cur->type != T_PIPE)
	{
		if (cur->type == T_WORD)
			argc++;
		if (ft_isredirection(cur->type) && cur->next)
			cur = cur->next;
		cur = cur->next;
	}
	return (argc);
}

static int	fill_argv(t_token *tokens, t_cmd *cmd)
{
	t_token	*cur;
	int		i;

	i = 0;
	cur = tokens;
	while (cur && cur->type != T_PIPE)
	{
		if (cur->type == T_WORD)
		{
			cmd->argv[i] = ft_strdup(cur->value);
			if (!cmd->argv[i])
				return (-1);
			cmd->argv[++i] = NULL;
		}
		if (ft_isredirection(cur->type))
			cur = cur->next;
		cur = cur->next;
	}
	return (0);
}

int	build_simple_cmd(t_token *tokens, t_cmd *cmd)
{
	int	argc;

	if (!cmd)
		return (-1);
	ft_bzero(cmd, sizeof(*cmd));
	if (parse_redirections(tokens, cmd) == -1)
	{
		ft_free_cmd(cmd);
		return (-1);
	}
	argc = count_args(tokens);
	cmd->argv = malloc(sizeof(char *) * (argc + 1));
	if (!cmd->argv)
	{
		ft_free_cmd(cmd);
		return (-1);
	}
	cmd->argv[0] = NULL;
	if (fill_argv(tokens, cmd) == -1)
	{
		ft_free_cmd(cmd);
		return (-1);
	}
	return (0);
}

int	execute_simple_cmd(t_data *data, t_cmd *cmd)
{
	pid_t	pid;
	int		status;
	char	*exec_path;
	char	**envp;
	int		fd;

	if (!data || !cmd || !cmd->argv || !cmd->argv[0])
		return (SUCCESS);

	exec_path = find_executable_path(data->env, cmd->argv[0]);
	if (!exec_path)
	{
		ft_putstr_fd("minishell: command not found: ", STDERR);
		ft_putstr_fd(cmd->argv[0], STDERR);
		ft_putstr_fd("\n", STDERR);
		data->status = 127;
		return (127);
	}

	envp = env_to_char_array(data->env);
	if (!envp)
	{
		free(exec_path);
		data->status = 1;
		return (ERROR);
	}

	pid = fork();
	if (pid < 0)
	{
		perror("fork");
		free(exec_path);
		ft_free_split(envp);
		data->status = ERROR;
		return (ERROR);
	}
	if (pid == 0)
	{
		if (cmd->infile)
		{
			fd = open(cmd->infile, O_RDONLY);
			if (fd < 0)
			{
				perror(cmd->infile);
				free(exec_path);
				ft_free_split(envp);
				exit(1);
			}
			dup2(fd, STDIN_FILENO);
			close(fd);
		}
		if (cmd->outfile)
		{
			fd = open(cmd->outfile,
					O_WRONLY | O_CREAT | (cmd->append ? O_APPEND : O_TRUNC),
					0644);
			if (fd < 0)
			{
				perror(cmd->outfile);
				free(exec_path);
				ft_free_split(envp);
				exit(1);
			}
			dup2(fd, STDOUT_FILENO);
			close(fd);
		}
		execve(exec_path, cmd->argv, envp);
		perror("execve");
		free(exec_path);
		ft_free_split(envp);
		exit(127);
	}
	waitpid(pid, &status, 0);

	free(exec_path);
	ft_free_split(envp);

	if (WIFEXITED(status))
		data->status = WEXITSTATUS(status);
	else
		data->status = 128 + WTERMSIG(status);
	return (SUCCESS);
}
