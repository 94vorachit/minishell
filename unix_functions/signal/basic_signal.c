/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   basic_signal.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/26 01:35:17 by vorhansa          #+#    #+#             */
/*   Updated: 2026/10/26 01:39:22 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <signal.h>
#include <stdio.h>
#include <unistd.h>

void	handle_sigint(int signum)
{
    (void)signum;
    printf("\nYou pressed Ctrl+C\n");
}

int	main(void)
{
    signal(SIGINT, handle_sigint); //interrupt
    // signal(SIGQUIT, handle_sigint); //quit

    while (1)
    {
        printf("Running...\n");
        sleep(1);
    }

    return (0);
}
