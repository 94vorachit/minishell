/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   basic_waitpid.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/26 01:28:12 by vorhansa          #+#    #+#             */
/*   Updated: 2026/10/26 01:28:15 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int	main(void)
{
    pid_t	pid;
    int		status;

    pid = fork();
    if (pid == -1)
    {
        perror("fork");
        return (1);
    }

    if (pid == 0)
    {
        printf("Child process\n");
        return (42);
    }

    printf("Parent is waiting...\n");
    waitpid(pid, &status, 0);

    if (WIFEXITED(status))
        printf("Child exited with status: %d\n", WEXITSTATUS(status));

    return (0);
}
