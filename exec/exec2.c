/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec2.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/03 15:29:46 by vorhansa          #+#    #+#             */
/*   Updated: 2026/11/04 12:51:44 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/**
 * Runs the external command in the child and exits with the proper status.
 */
void	exec_external_child(t_cmd *cmd, char *exec_path, char **envp)
{
	if (cmd->infile && redirect_input(cmd) == -1)
		exit(1);
	if (cmd->outfile && redirect_output(cmd) == -1)
		exit(1);
	execve(exec_path, cmd->argv, envp);
	perror(exec_path);
	exit(126);
}

/**
 * Executes an external command and waits for it to finish.
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
	// while (waitpid(status, &child_status, 0) == -1)
	// {
	// 	if (errno != EINTR)
	// 	{
	// 		perror("waitpid");
	// 		sig_init();
	// 		free(exec_path);
	// 		ft_free_split(envp);
	// 		data->status = 1;
	// 		return (ERROR);
	// 	}
	// }
	sig_init();
	free(exec_path);
	ft_free_split(envp);
	set_child_status(data, child_status);
	return (data->status);
}

/**
 * Entry point for one shell command.
 */
int	execute_simple_cmd(t_data *data, t_cmd *cmd)
{
	if (!data || !cmd || !cmd->argv || !cmd->argv[0])
		return (SUCCESS);
	if (cmd->argv[0][0] == '\0')
		return (SUCCESS);
	if (is_builtin(cmd->argv[0]))
		return (run_builtin_cmd(data, cmd));
	return (run_external_cmd(data, cmd));
}
