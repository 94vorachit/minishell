/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execve_fork.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/26 02:00:17 by vorhansa          #+#    #+#             */
/*   Updated: 2026/10/26 02:04:55 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>

int	main(void)
{
    pid_t	pid;
    int		status;
    char	*args[] = {"ls", "-l", NULL};
    char	*envp[] = {NULL};

    pid = fork();
    if (pid == -1)
    {
        perror("fork");
        return (1);
    }

    if (pid == 0)
    {
        /* Child process becomes ls */
        printf("Child process becomes ls\n");
        execve("/bin/ls", args, envp);
        perror("execve");
        exit(127);
    }

    /* Parent waits for ls */
    printf("Parent waits for ls\n");
    waitpid(pid, &status, 0);
    return (0);
}
