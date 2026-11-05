/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/28 19:36:16 by vorhansa          #+#    #+#             */
/*   Updated: 2026/11/05 17:13:06 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/**
 * Prepares everything before `fork()`.
 * 
 * 1. Finds the executable path with `find_executable_path(...)`
 * 2. Prints:
 * `minishell: command not found: <name>` 
 * when no executable is found
 * 3. Saves exit status as `127`
 * 4. Builds environment array with `env_to_char_array(...)`
 * 
 * Returns:
 * * `0` on success
 * * `-1` on error
 */
int	prepare_execution(t_data *data, t_cmd *cmd,
				char **exec_path, char ***envp)
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
 * Runs the actual command in the child process.
 * 
 * Behavior:
 * 1. If `cmd->infile` exists, call `redirect_input`
 * 2. If `cmd->outfile` exists, call `redirect_output`
 * 3. Then run:
 * `execve(exec_path, cmd->argv, envp)`
 * 4. If `execve` fails, print `perror("execve")` and exit with `127`
 * 
 * This is where the process actually becomes the target command.
 */
void	run_child(t_cmd *cmd, char *exec_path, char **envp)
{
	if ((cmd->infile || cmd->heredoc_fd != -1) && redirect_input(cmd) == -1)
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
	perror(exec_path);
	free(exec_path);
	ft_free_split(envp);
	exit(126);
}

/**
 * Converts the child exit status into the shell status.
 * 
 * The exit status is converted from `waitpid` using the normal shell rule:
 * - `Ctrl+C` -> 130
 * - `Ctrl+\` -> 131
 *
 * Behavior:
 * * If child exited normally:
 * `data->status = WEXITSTATUS(status)`
 * * Else if it was terminated by a signal:
 * `data->status = 128 + WTERMSIG(status)`
 */
void	set_child_status(t_data *data, int status)
{
	if (WIFEXITED(status))
	{
		data->status = WEXITSTATUS(status);
		g_signal = 0;
		return ;
	}
	if (WIFSIGNALED(status))
	{
		if (WTERMSIG(status) == SIGINT)
			data->status = 130;
		else if (WTERMSIG(status) == SIGQUIT)
			data->status = 131;
		else
			data->status = 128 + WTERMSIG(status);
		g_signal = 0;
		return ;
	}
	data->status = 1;
	g_signal = 0;
}

/**
 * Forks and waits for the command.
 * 
 * Flow:
 * 1. `fork()`
 * 2. Child: `run_child(...)`
 * 3. Parent: `waitpid(pid, &status, 0)`
 * 4. Free memory
 * 5. Call `set_child_status(...)`
 * 
 * This is the execution wrapper around the child.
 */
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
	// while (waitpid(pid, &status, 0) == -1)
	// {
	// 	if (errno != EINTR)
	// 	{
	// 		perror("waitpid");
	// 		free(exec_path);
	// 		ft_free_split(envp);
	// 		data->status = 1;
	// 		return (ERROR);
	// 	}
	// }
	free(exec_path);
	ft_free_split(envp);
	set_child_status(data, status);
	return (data->status);
}

/**
 * Executes a built-in command with a temporary dup of stdin/stdout.
 */
int	run_builtin_cmd(t_data *data, t_cmd *cmd)
{
	int	in_fd;
	int	out_fd;
	int	ret;

	in_fd = dup(STDIN_FILENO);
	out_fd = dup(STDOUT_FILENO);
	if (in_fd < 0 || out_fd < 0)
	{
		if (in_fd >= 0)
			close(in_fd);
		if (out_fd >= 0)
			close(out_fd);
		perror("dup");
		data->status = ERROR;
		return (ERROR);
	}
	if (((cmd->infile || cmd->heredoc_fd != -1) && redirect_input(cmd) == -1)
		|| (cmd->outfile && redirect_output(cmd) == -1))
	{
		dup2(in_fd, STDIN_FILENO);
		dup2(out_fd, STDOUT_FILENO);
		close(in_fd);
		close(out_fd);
		data->status = ERROR;
		return (ERROR);
	}
	ret = execute_builtin(data, cmd);
	dup2(in_fd, STDIN_FILENO);
	dup2(out_fd, STDOUT_FILENO);
	close(in_fd);
	close(out_fd);
	data->status = ret;
	return (ret);
}
