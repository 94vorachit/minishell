/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   basic_pipe.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/26 02:07:58 by vorhansa          #+#    #+#             */
/*   Updated: 2026/10/26 02:11:15 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdio.h>

int	main(void)
{
    int	pipefd[2];
    char	buffer[100];
    int		bytes_read;

    if (pipe(pipefd) == -1)
    {
        perror("pipe");
        return (1);
    }

    write(pipefd[1], "write Hello from pipefd[1]\n", 27);

    bytes_read = read(pipefd[0], buffer, sizeof(buffer) - 1);
    buffer[bytes_read] = '\0';

    printf("read Received from pipefd[0]: %s", buffer);

    close(pipefd[0]);
    close(pipefd[1]);

    return (0);
}
