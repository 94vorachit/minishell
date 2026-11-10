/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal2.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/26 19:15:45 by vorhansa          #+#    #+#             */
/*   Updated: 2026/11/10 15:57:08 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/**
 * Restore the default handling for child processes.
 *
 * External commands should not inherit the shell's prompt-level handlers. This
 * function resets `SIGINT` and `SIGQUIT` to the OS defaults so children behave
 * like normal processes under bash.
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
 * Ignore `SIGINT` and `SIGQUIT` in the parent shell during a child-run window.
 *
 * This prevents the parent shell from reacting to prompt-level signals while it
 * is waiting on a child process or a heredoc subprocess. The shell resumes its
 * normal handling once the child is done.
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
 * Configure signal behavior for heredoc subprocesses.
 *
 * A heredoc child should stop on `SIGINT` in the normal way so the parent can
 * detect the interruption through `waitpid()`. `SIGQUIT` remains ignored to
 * match the shell's prompt-level policy and avoid spurious core-dumps.
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
