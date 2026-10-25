/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   basic_dup.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/26 02:43:36 by vorhansa          #+#    #+#             */
/*   Updated: 2026/10/26 02:50:25 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int	main(void)
{
    int	old_fd;
    int	new_fd;

    old_fd = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (old_fd == -1)
    {
        perror("open");
        return (1);
    }

    /*
    int new_fd;

    new_fd = dup(old_fd);
    */
    new_fd = dup(old_fd);
    if (new_fd == -1)
    {
        perror("dup");
        close(old_fd);
        return (1);
    }

    write(new_fd, "old_fd = open(\"output.txt\", O_WRONLY | O_CREAT | O_TRUNC, 0644)\nwrite(new_fd, \"Hello\", 6)\nHello\n", 96);

    close(old_fd);
    close(new_fd);
    return (0);
}
