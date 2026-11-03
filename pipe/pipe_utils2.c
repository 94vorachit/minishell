/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_utils2.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/03 14:32:23 by vorhansa          #+#    #+#             */
/*   Updated: 2026/11/03 18:23:56 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/**
 * Counts how many commands are in the linked list
 */
int	count_commands(t_cmd *cmd)
{
	int	count;

	count = 0;
	while (cmd)
	{
		count++;
		cmd = cmd->next;
	}
	return (count);
}

/**
 * Connects the child process to the correct pipe ends
 * 
 * Behavior:
 * 1. if there is a previous pipe (`prev_fd != -1`):
 * * duplicate `prev_fd` to `STDIN_FILENO`
 * * close `prev_fd`
 * 2. if this command has a next command (`cur->next`):
 * * duplicate `pipefd[1]` to `STDOUT_FILENO`
 * * close `pipefd[0]` and `pipefd[1]`
 * 
 * This is the core of pipeline I/O wiring:
 * * previous command writes to this command’s stdin
 * * this command writes to next command’s stdin through the pipe
 */
void	setup_child_io(t_cmd *cur, int prev_fd, int pipefd[2])
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
}

/**
 * Applies the command’s own redirections inside the child
 * 
 * Behavior:
 * 1. if cur->infile exists, call redirect_input(cur)
 * 2. if cur->outfile exists, call redirect_output(cur)
 * 3. returns -1 on failure
 * 
 * This lets a command like:
 * `cat < infile | wc > out`
 * 
 * redirect its input/output correctly before execution.
 */
int	redirect_child_cmd(t_cmd *cur)
{
	if (cur->infile && redirect_input(cur) == -1)
		return (-1);
	if (cur->outfile && redirect_output(cur) == -1)
		return (-1);
	return (0);
}

/**
 * Updates the parent side of the pipeline after forking a child
 * 
 * Behavior:
 * 1. closes the old prev_fd if it exists
 * 2. if the current command has a next command:
 * * close pipefd[1]
 * * set *prev_fd = pipefd[0]
 * 
 * This keeps the parent’s pipe state moving forward through the pipeline.
 */
void	update_parent_pipe(int *prev_fd, t_cmd *cur, int pipefd[2])
{
	if (*prev_fd != -1)
		close(*prev_fd);
	if (cur->next)
	{
		close(pipefd[1]);
		*prev_fd = pipefd[0];
	}
}

/**
 * The actual child logic for one pipeline command
 * 
 * Behavior:
 * 1. setup_child_io(...)
 * 2. redirect_child_cmd(...)
 * 3. if command is builtin:
 * * exit(execute_builtin(data, cur));
 * 4. else:
 * * find executable path
 * * build envp
 * * if path or env is missing, exit with 127
 * 5. execve(path, cur->argv, envp);
 * 6. if execve fails:
 * * print perror("execve")
 * * exit with 127
 * 
 * This function is the child process execution step for 
 * each pipeline command.
 */
void	run_child_cmd(t_data *data, t_cmd *cur, int prev_fd, int pipefd[2])
{
	char	*path;
	char	**envp;

	sig_default();
	setup_child_io(cur, prev_fd, pipefd);
	if (redirect_child_cmd(cur) == -1)
		exit(ERROR);
	if (is_builtin(cur->argv[0]))
		exit(execute_builtin(data, cur));
	path = find_executable_path(data->env, cur->argv[0]);
	envp = env_to_char_array(data->env);
	if (!path || !envp)
		exit(UNKNOWN_COMMAND);
	execve(path, cur->argv, envp);
	perror("execve");
	exit(UNKNOWN_COMMAND);
}
