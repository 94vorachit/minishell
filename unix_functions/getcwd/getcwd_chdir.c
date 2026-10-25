/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   getcwd_chdir.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/26 01:23:15 by vorhansa          #+#    #+#             */
/*   Updated: 2026/10/26 01:23:18 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int	main(void)
{
    char	*directory;

    directory = getcwd(NULL, 0);
    printf("Before: %s\n", directory);
    free(directory);

    if (chdir("/tmp") == -1)
    {
        perror("chdir");
        return (1);
    }

    directory = getcwd(NULL, 0);
    if (directory == NULL)
    {
        perror("getcwd");
        return (1);
    }

    printf("After:  %s\n", directory);
    free(directory);

    return (0);
}
