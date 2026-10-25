/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell_signal.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/26 01:40:31 by vorhansa          #+#    #+#             */
/*   Updated: 2026/10/26 01:46:22 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <signal.h>
#include <stdio.h>
#include <unistd.h>

void	handle_sigint(int signum)
{
    (void)signum;
    printf("minishell> ");
    write(STDOUT_FILENO, "\nminishell> \n", 13);
}

int	main(void)
{
    signal(SIGINT, handle_sigint);

    while (1)
    {
        /* Read and execute shell commands */
    }

    return (0);
}
