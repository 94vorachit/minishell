/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   basic_dup2.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/26 02:51:40 by vorhansa          #+#    #+#             */
/*   Updated: 2026/10/26 02:52:36 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int	main(void)
{
    int	file_fd;

    file_fd = open("output_dup2.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (file_fd == -1)
    {
        perror("open");
        return (1);
    }

    if (dup2(file_fd, STDOUT_FILENO) == -1)
    {
        perror("dup2");
        close(file_fd);
        return (1);
    }

    close(file_fd);
    printf("This goes into output_dup2.txt\n");

    return (0);
}
