/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_fork.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/26 02:11:42 by vorhansa          #+#    #+#             */
/*   Updated: 2026/10/26 02:14:11 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>

int	main(void)
{
    int		pipefd[2];
    pid_t	pid;
    char	buffer[100];
    int		bytes_read;

    if (pipe(pipefd) == -1)
    {
        perror("pipe");
        return (1);
    }

    pid = fork();
    if (pid == -1)
    {
        perror("fork");
        return (1);
    }

    if (pid == 0)
    {
        /* Child writes into the pipe */
        close(pipefd[0]);
        write(pipefd[1], "write message from child pipefd[1]\n", 35);
        close(pipefd[1]);
        exit(0);
    }
    else
    {
        /* Parent reads from the pipe */
        close(pipefd[1]);
        bytes_read = read(pipefd[0], buffer, sizeof(buffer) - 1);
        buffer[bytes_read] = '\0';
        printf("read message from Parent pipefd[0] received: %s", buffer);
        close(pipefd[0]);
        waitpid(pid, NULL, 0);
    }

    return (0);
}
