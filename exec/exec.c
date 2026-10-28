/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/28 19:36:16 by vorhansa          #+#    #+#             */
/*   Updated: 2026/10/29 03:29:40 by vorhansa         ###   ########.fr       */
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

int	fill_argv(t_token *tokens, t_cmd *cmd)
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

int	prepare_execution(t_data *data, t_cmd *cmd,
				char **exec_path, char ***envp)
{
	*exec_path = find_executable_path(data->env, cmd->argv[0]);
	if (!*exec_path)
	{
		ft_putstr_fd("minishell: command not found: ", STDERR);
		ft_putstr_fd(cmd->argv[0], STDERR);
		ft_putstr_fd("\n", STDERR);
		data->status = 127;
		return (-1);
	}
	*envp = env_to_char_array(data->env);
	if (!*envp)
	{
		free(*exec_path);
		data->status = 1;
		return (-1);
	}
	return (0);
}

int	redirect_input(t_cmd *cmd)
{
	int	fd;

	fd = open(cmd->infile, O_RDONLY);
	if (fd < 0)
	{
		perror(cmd->infile);
		return (-1);
	}
	dup2(fd, STDIN_FILENO);
	close(fd);
	return (0);
}

int	redirect_output(t_cmd *cmd)
{
	int	fd;

	fd = open(cmd->outfile, O_WRONLY | O_CREAT
			| (cmd->append ? O_APPEND : O_TRUNC), 0644);
	if (fd < 0)
	{
		perror(cmd->outfile);
		return (-1);
	}
	dup2(fd, STDOUT_FILENO);
	close(fd);
	return (0);
}

void	run_child(t_cmd *cmd, char *exec_path, char **envp)
{
	if (cmd->infile && redirect_input(cmd) == -1)
	{
		free(exec_path);
		ft_free_split(envp);
		exit(1);
	}
	if (cmd->outfile && redirect_output(cmd) == -1)
	{
		free(exec_path);
		ft_free_split(envp);
		exit(1);
	}
	execve(exec_path, cmd->argv, envp);
	perror("execve");
	free(exec_path);
	ft_free_split(envp);
	exit(127);
}

void	set_child_status(t_data *data, int status)
{
	if (WIFEXITED(status))
		data->status = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
		data->status = 128 + WTERMSIG(status);
}

int	run_process(t_data *data, t_cmd *cmd, char *exec_path, char **envp)
{
	pid_t	pid;
	int		status;

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
		run_child(cmd, exec_path, envp);
	waitpid(pid, &status, 0);
	free(exec_path);
	ft_free_split(envp);
	set_child_status(data, status);
	return (SUCCESS);
}

int	execute_simple_cmd(t_data *data, t_cmd *cmd)
{
	char	*exec_path;
	char	**envp;

	if (!data || !cmd || !cmd->argv || !cmd->argv[0])
		return (SUCCESS);
	if (prepare_execution(data, cmd, &exec_path, &envp) == -1)
		return (data->status);
	return (run_process(data, cmd, exec_path, envp));
}
