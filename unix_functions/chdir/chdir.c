/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   chdir.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/25 22:54:39 by vorhansa          #+#    #+#             */
/*   Updated: 2026/10/25 22:55:00 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int	main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Usage: %s <directory>\n", argv[0]);
        return (1);
    }

    if (chdir(argv[1]) == -1)
    {
        perror("chdir");
        return (1);
    }

    printf("Directory changed successfully\n");

    return (0);
}

/*
gcc main.c -o change_dir
./change_dir /tmp
*/