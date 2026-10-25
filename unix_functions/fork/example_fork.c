/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   example_fork.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/26 01:51:42 by vorhansa          #+#    #+#             */
/*   Updated: 2026/10/26 01:56:29 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdio.h>

int	main(void)
{
    pid_t	pid;

    pid = fork();

    if (pid == -1)
    {
        perror("fork");
        return (1);
    }

    if (pid == 0)
    {
        printf("This is the *child* process Child PID: %d\n", getpid());
        printf("This is the *child* process Parent PID: %d\n", getppid());
    }
    else
    {
        printf("This is the *parent* process Parent PID: %d\n", getpid());
        printf("This is the *parent* process Child PID: %d\n", pid);
    }

    return (0);
}
