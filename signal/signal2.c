/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal2.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/26 19:15:45 by vorhansa          #+#    #+#             */
/*   Updated: 2026/11/10 14:45:18 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/**
 * Restores the default signal behavior for child processes.
 *
 * This is used before executing external programs so they can handle
 * `SIGINT` and `SIGQUIT` the same way a normal shell would.
 */
void	sig_default(void)
{
	struct sigaction	int_act;
	struct sigaction	quit_act;

	sigemptyset(&int_act.sa_mask);
	sigemptyset(&quit_act.sa_mask);
	int_act.sa_handler = SIG_DFL;
	quit_act.sa_handler = SIG_DFL;
	int_act.sa_flags = 0;
	quit_act.sa_flags = 0;
	sigaction(SIGINT, &int_act, NULL);
	sigaction(SIGQUIT, &quit_act, NULL);
}

/**
 * Ignores `SIGINT` and `SIGQUIT` for a short execution window.
 *
 * This is typically used while spawning a subprocess or heredoc child, so the
 * parent shell does not react to the signals while the child is running.
 */
void	sig_ignore(void)
{
	struct sigaction	int_act;
	struct sigaction	quit_act;

	sigemptyset(&int_act.sa_mask);
	sigemptyset(&quit_act.sa_mask);
	int_act.sa_handler = SIG_IGN;
	quit_act.sa_handler = SIG_IGN;
	int_act.sa_flags = 0;
	quit_act.sa_flags = 0;
	sigaction(SIGINT, &int_act, NULL);
	sigaction(SIGQUIT, &quit_act, NULL);
}

/**
 * Configures heredoc signal behavior.
 *
 * - `SIGINT` uses default handling so the heredoc child is interrupted
 *   normally and the parent can detect it through `waitpid()`
 * - `SIGQUIT` is ignored
 */
void	sig_heredoc(void)
{
	struct sigaction	int_act;
	struct sigaction	quit_act;

	sigemptyset(&int_act.sa_mask);
	sigemptyset(&quit_act.sa_mask);
	int_act.sa_handler = SIG_DFL;
	quit_act.sa_handler = SIG_IGN;
	int_act.sa_flags = 0;
	quit_act.sa_flags = 0;
	sigaction(SIGINT, &int_act, NULL);
	sigaction(SIGQUIT, &quit_act, NULL);
}
