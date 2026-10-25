/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   basic_execve.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/26 01:58:55 by vorhansa          #+#    #+#             */
/*   Updated: 2026/10/26 01:58:59 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdio.h>

int	main(void)
{
    char	*args[] = {"ls", "-l", NULL};
    char	*envp[] = {NULL};

    execve("/bin/ls", args, envp);

    /* This runs only if execve fails */
    perror("execve");
    return (1);
}
