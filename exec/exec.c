/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/28 19:36:16 by vorhansa          #+#    #+#             */
/*   Updated: 2026/11/02 18:19:43 by vorhansa         ###   ########.fr       */
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
	{
		int	in_fd;
		int	out_fd;

		in_fd = dup(STDIN_FILENO);
		out_fd = dup(STDOUT_FILENO);
		if (cmd->infile && redirect_input(cmd) == -1)
			return (ERROR);
		if (cmd->outfile && redirect_output(cmd) == -1)
			return (ERROR);
		data->status = execute_builtin(data, cmd);
		dup2(in_fd, STDIN_FILENO);
		dup2(out_fd, STDOUT_FILENO);
		close(in_fd);
		close(out_fd);
		return (data->status);
	}
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
		return (1);
	}
	if (fork() == 0)
	{
		if (cmd->infile && redirect_input(cmd) == -1)
			exit(1);
		if (cmd->outfile && redirect_output(cmd) == -1)
			exit(1);
		execve(exec_path, cmd->argv, envp);
		perror("execve");
		exit(127);
	}
	waitpid(-1, NULL, 0);
	free(exec_path);
	ft_free_split(envp);
	return (SUCCESS);
}

int	execute_pipeline(t_data *data, t_cmd *cmd)
{
	int		pipefd[2];
	int		prev_fd;
	pid_t	pid;
	t_cmd	*cur;
	int		status;

	prev_fd = -1;
	cur = cmd;
	while (cur)
	{
		if (cur->next && pipe(pipefd) < 0)
		{
			perror("pipe");
			return (ERROR);
		}
		pid = fork();
		if (pid == 0)
		{
			if (prev_fd != -1)
			{
				dup2(prev_fd, STDIN_FILENO);
				close(prev_fd);
			}
			if (cur->next)
			{
				dup2(pipefd[1], STDOUT_FILENO);
				close(pipefd[0]);
				close(pipefd[1]);
			}
			if (cur->infile && redirect_input(cur) == -1)
				exit(1);
			if (cur->outfile && redirect_output(cur) == -1)
				exit(1);
			if (is_builtin(cur->argv[0]))
				exit(execute_builtin(data, cur));
			else
			{
				char	*path;
				char	**envp;

				path = find_executable_path(data->env, cur->argv[0]);
				envp = env_to_char_array(data->env);
				if (!path || !envp)
					exit(127);
				execve(path, cur->argv, envp);
				perror("execve");
				exit(127);
			}
		}
		if (prev_fd != -1)
			close(prev_fd);
		if (cur->next)
		{
			close(pipefd[1]);
			prev_fd = pipefd[0];
		}
		waitpid(pid, &status, 0);
		set_child_status(data, status);
		cur = cur->next;
	}
	return (data->status);
}
