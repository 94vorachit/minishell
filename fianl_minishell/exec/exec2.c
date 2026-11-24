/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec2.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/03 15:29:46 by vorhansa          #+#    #+#             */
/*   Updated: 2026/11/23 15:15:42 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/**
 * Purpose: print the standard shell error for unresolved executable names.
 *
 * How it works:
 * 1. if command contains '/', prints no-such-file-or-directory prefix
 * 2. otherwise prints command-not-found prefix
 * 3. appends the command name and newline to STDERR
 */
void	ft_print_exec_error(char *cmd)
{
	if (ft_strchr(cmd, '/'))
	{
		ft_putstr_fd(cmd, STDERR);
		ft_putstr_fd(": No such file or directory\n", STDERR);
	}
	else
	{
		ft_putstr_fd(cmd, STDERR);
		ft_putstr_fd(": command not found\n", STDERR);
	}
}

/**
 * Purpose: resolve the real path of the command and build `envp`.
 * 
 * How it works:
 * 1. `ft_find_exec_path(data->env, cmd->argv[0])`
 * 2. if not found, prints:
 * * `command not found`
 * * or `no such file or directory`
 * 3. calls `ft_env_to_array(data->env)` to convert environment into `char **`
 */
int	ft_prepare_exec(t_data *data, t_cmd *cmd, char **exec_path, char ***envp)
{
	*exec_path = ft_find_exec_path(data->env, cmd->argv[0]);
	if (!*exec_path)
	{
		ft_print_exec_error(cmd->argv[0]);
		data->status = 127;
		return (-1);
	}
	*envp = ft_env_to_array(data->env);
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
void	ft_exec_external_child(t_cmd *cmd, char *exec_path, char **envp)
{
	if ((cmd->infile || cmd->heredoc_fd != -1) && ft_redir_input(cmd) == -1)
		exit(1);
	if (cmd->outfile && ft_redir_output(cmd) == -1)
		exit(1);
	if (ft_is_directory_path(exec_path))
	{
		ft_putstr_fd("minishell: ", STDERR);
		ft_putstr_fd(exec_path, STDERR);
		ft_putstr_fd(": Is a directory\n", STDERR);
		exit(126);
	}
	execve(exec_path, cmd->argv, envp);
	perror(exec_path);
	exit(126);
}

/**
 * Purpose: execute and wait for an external command.
 *
 * How it works:
 * 1. resolves the executable and environment
 * 2. changes parent signal handling for the child-run window
 * 3. forks and gives the child default signal handlers
 * 4. waits for the child and detects SIGINT or SIGQUIT
 * 5. prints `^C\n` or `^\\\n` for a signaled child
 * 6. restores prompt handlers and stores the final status
 */
int	ft_run_external_cmd(t_data *data, t_cmd *cmd)
{
	char	*exec_path;
	char	**envp;
	int		status;
	int		child_status;

	if (ft_prepare_exec(data, cmd, &exec_path, &envp) == -1)
		return (data->status);
	ft_sig_ignore();
	status = fork();
	if (status == 0)
	{
		ft_sig_default();
		ft_exec_external_child(cmd, exec_path, envp);
	}
	waitpid(status, &child_status, 0);
	if (WIFSIGNALED(child_status) && WTERMSIG(child_status) == SIGINT)
		write(STDERR, "^C\n", 3);
	else if (WIFSIGNALED(child_status) && WTERMSIG(child_status) == SIGQUIT)
		write(STDERR, "^\\\n", 3);
	ft_sig_init();
	free(exec_path);
	ft_free_split(envp);
	ft_set_child_status(data, child_status);
	return (data->status);
}

/**
 * Purpose: top-level entry for one command.
 * 
 * How it works:
 * 1. checks if command is valid
 * 2. runs heredoc preparation
 * 3. if command name is a builtin, calls `ft_run_builtin_cmd`
 * 4. otherwise calls `ft_run_external_cmd`
 */
int	ft_exec_simple_cmd(t_data *data, t_cmd *cmd)
{
	if (!data || !cmd)
		return (ERROR);
	if (ft_prepare_heredoc_list(cmd) == -1)
	{
		if (g_signal == SIGINT || g_signal == SIGQUIT)
			data->status = 128 + g_signal;
		else
			data->status = 130;
		return (data->status);
	}
	if (!cmd->argv || !cmd->argv[0])
		return (SUCCESS);
	if (ft_is_builtin(cmd->argv[0]))
		return (ft_run_builtin_cmd(data, cmd));
	return (ft_run_external_cmd(data, cmd));
}
