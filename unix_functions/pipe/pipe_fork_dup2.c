/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_fork_dup2.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/26 02:15:42 by vorhansa          #+#    #+#             */
/*   Updated: 2026/10/26 02:16:16 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>

int	main(void)
{
	int		pipefd[2];
	pid_t	pid1;
	pid_t	pid2;
	int		status1;
	int		status2;
	char	*args1[] = {"ls", "-l", NULL};
	char	*args2[] = {"grep", "\\.c$", NULL};
	extern char	**environ;

	if (pipe(pipefd) == -1)
	{
		perror("pipe");
		return (1);
	}
	pid1 = fork();
	if (pid1 == -1)
	{
		perror("fork");
		close(pipefd[0]);
		close(pipefd[1]);
		return (1);
	}
	if (pid1 == 0)
	{
		close(pipefd[0]);
		if (dup2(pipefd[1], STDOUT_FILENO) == -1)
		{
			perror("dup2");
			close(pipefd[1]);
			return (127);
		}
		close(pipefd[1]);
		execve("/bin/ls", args1, environ);
		perror("execve ls");
		return (127);
	}
	pid2 = fork();
	if (pid2 == -1)
	{
		perror("fork");
		close(pipefd[0]);
		close(pipefd[1]);
		waitpid(pid1, NULL, 0);
		return (1);
	}
	if (pid2 == 0)
	{
		close(pipefd[1]);
		if (dup2(pipefd[0], STDIN_FILENO) == -1)
		{
			perror("dup2");
			close(pipefd[0]);
			return (127);
		}
		close(pipefd[0]);
		execve("/bin/grep", args2, environ);
		perror("execve grep");
		return (127);
	}
	close(pipefd[0]);
	close(pipefd[1]);
	if (waitpid(pid1, &status1, 0) == -1)
	{
		perror("waitpid");
		return (1);
	}
	if (waitpid(pid2, &status2, 0) == -1)
	{
		perror("waitpid");
		return (1);
	}
	if (WIFEXITED(status2))
		return (WEXITSTATUS(status2));
	if (WIFSIGNALED(status2))
		return (128 + WTERMSIG(status2));
	return (1);
}
