/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec2.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/03 15:29:46 by vorhansa          #+#    #+#             */
/*   Updated: 2026/11/09 17:15:54 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/**
 * Purpose: resolve the real path of the command and build `envp`.
 * 
 * How it works:
 * 1. `find_executable_path(data->env, cmd->argv[0])`
 * 2. if not found, prints:
 * * `command not found`
 * * or `no such file or directory`
 * 3. calls `env_to_char_array(data->env)` to convert environment into `char **`
 */
int	prepare_execution(t_data *data, t_cmd *cmd, char **exec_path, char ***envp)
{
	*exec_path = find_executable_path(data->env, cmd->argv[0]);
	if (!*exec_path)
	{
		if (ft_strchr(cmd->argv[0], '/'))
			ft_putstr_fd("minishell: no such file or directory: ", STDERR);
		else
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

/**
 * Purpose: child process body for external commands.
 * 
 * How it works:
 * 1. applies input/output redirection
 * 2. calls `execve(exec_path, cmd->argv, envp)`
 * 3. if `execve` fails, prints error and exits with code 126
 */
void	exec_external_child(t_cmd *cmd, char *exec_path, char **envp)
{
	if ((cmd->infile || cmd->heredoc_fd != -1) && redirect_input(cmd) == -1)
		exit(1);
	if (cmd->outfile && redirect_output(cmd) == -1)
		exit(1);
	execve(exec_path, cmd->argv, envp);
	perror(exec_path);
	exit(126);
}

/**
 * Purpose: execute a non-builtin command.
 * 
 * How it works:
 * 1. calls `prepare_execution`
 * 2. forks a child process
 * 3. in child: calls `exec_external_child`
 * 4. parent waits with `waitpid`
 * 5. converts child status with `set_child_status`
 */
int	run_external_cmd(t_data *data, t_cmd *cmd)
{
	char	*exec_path;
	char	**envp;
	int		status;
	int		child_status;

	if (prepare_execution(data, cmd, &exec_path, &envp) == -1)
		return (data->status);
	sig_ignore();
	status = fork();
	if (status == 0)
	{
		sig_default();
		exec_external_child(cmd, exec_path, envp);
	}
	waitpid(status, &child_status, 0);
	sig_init();
	free(exec_path);
	ft_free_split(envp);
	set_child_status(data, child_status);
	return (data->status);
}

/**
 * Purpose: top-level entry for one command.
 * 
 * How it works:
 * 1. checks if command is valid
 * 2. runs heredoc preparation
 * 3. if command name is a builtin, calls `run_builtin_cmd`
 * 4. otherwise calls `run_external_cmd`
 */
int	execute_simple_cmd(t_data *data, t_cmd *cmd)
{
	if (!data || !cmd || !cmd->argv || !cmd->argv[0])
		return (SUCCESS);
	if (cmd->argv[0][0] == '\0')
		return (SUCCESS);
	if (prepare_heredoc_list(cmd) == -1)
	{
		data->status = 130;
		return (data->status);
	}
	if (is_builtin(cmd->argv[0]))
		return (run_builtin_cmd(data, cmd));
	return (run_external_cmd(data, cmd));
}

/*
int	run_external_cmd(t_data *data, t_cmd *cmd)
{
	char	*exec_path;
	char	**envp;
	int		status;
	int		child_status;

	if (prepare_execution(data, cmd, &exec_path, &envp) == -1)
		return (data->status);
	sig_ignore();
	status = fork();
	if (status == 0)
	{
		sig_default();
		exec_external_child(cmd, exec_path, envp);
	}
	// waitpid(status, &child_status, 0);
	while (waitpid(status, &child_status, 0) == -1)
	{
		if (errno != EINTR)
		{
			perror("waitpid");
			sig_init();
			free(exec_path);
			ft_free_split(envp);
			data->status = 1;
			return (ERROR);
		}
	}
	sig_init();
	free(exec_path);
	ft_free_split(envp);
	set_child_status(data, child_status);
	return (data->status);
}*/