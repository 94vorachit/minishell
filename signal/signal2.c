/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal2.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/26 19:15:45 by vorhansa          #+#    #+#             */
/*   Updated: 2026/10/28 16:53:46 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/**
 * Restores the default OS behavior for signals.
 * 
 * Meaning:
 * 1. Ctrl+C goes back to the normal shell behavior
 * 2. Ctrl+\ goes back to the normal shell behavior
 * 
 * This is useful when you want to let the child process or external command 
 * handle the signal normally.
 */
void	sig_default(void)
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
}

/**
 * Ignores both interactive interrupt signals.
 * 
 * Meaning:
 * 1. the shell does not react to Ctrl+C
 * 2. the shell does not react to Ctrl+\
 * 
 * This is used when the program wants to suppress signal interrupts temporarily.
 */
void	sig_ignore(void)
{
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
}

/**
 * This is the signal setup used during heredoc mode.
 * 
 * Meaning:
 * 1. Ctrl+C uses sig_interrupt
 * 2. Ctrl+\ uses sig_quit
 * 
 * This is a special mode where the shell is waiting for multi-line input, 
 * and the signal handlers are set to behave appropriately for that context.
 */
void	sig_heredoc(void)
{
	signal(SIGINT, sig_interrupt);
	signal(SIGQUIT, sig_quit);
}
