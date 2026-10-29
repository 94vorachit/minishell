/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/28 19:36:16 by vorhansa          #+#    #+#             */
/*   Updated: 2026/10/29 17:56:36 by vorhansa         ###   ########.fr       */
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

/**
 * Converts the child exit status into the shell status.
 * 
 * Behavior:
 * * If child exited normally:
 * `data->status = WEXITSTATUS(status)`
 * Else if it was terminated by a signal:
 * `data->status = 128 + WTERMSIG(status)`
 * 
 * This matches normal shell behavior.
 */
void	set_child_status(t_data *data, int status)
{
	if (WIFEXITED(status))
		data->status = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
		data->status = 128 + WTERMSIG(status);
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
	free(exec_path);
	ft_free_split(envp);
	set_child_status(data, status);
	return (SUCCESS);
}

/**
 * Entry point for one shell command.
 * 
 * Flow:
 * 1. Validate arguments
 * 2. Call prepare_execution(...)
 * 3. If preparation fails, return
 * 4. Otherwise call run_process(...)
 * 
 * This is the public function used by the rest of the shell.
 */
int	execute_simple_cmd(t_data *data, t_cmd *cmd)
{
	char	*exec_path;
	char	**envp;

	if (!data || !cmd || !cmd->argv || !cmd->argv[0])
		return (SUCCESS);
	if (is_builtin(cmd->argv[0]))
		return (execute_builtin(data, cmd));
	if (prepare_execution(data, cmd, &exec_path, &envp) == -1)
		return (data->status);
	return (run_process(data, cmd, exec_path, envp));
}
