/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal2.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/26 19:15:45 by vorhansa          #+#    #+#             */
/*   Updated: 2026/11/20 19:56:32 by vorhansa         ###   ########.fr       */
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
void	ft_sig_default(void)
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
 *
 * Purpose: configure the parent shell while it waits for a child process.
 *
 * How it works:
 * 1. installs ignore handlers for `SIGINT` and `SIGQUIT`
 * 2. disables `ECHOCTL` while the child is running
 * 3. prevents automatic terminal echo of control characters
 *
 * The shell restores prompt and ECHOCTL handling after `waitpid()` through
 * `ft_sig_init()`.
 */
void	ft_sig_ignore(void)
{
	struct sigaction	int_act;
	struct sigaction	quit_act;
	struct termios		termios_p;

	sigemptyset(&int_act.sa_mask);
	sigemptyset(&quit_act.sa_mask);
	int_act.sa_handler = SIG_IGN;
	quit_act.sa_handler = SIG_IGN;
	int_act.sa_flags = 0;
	quit_act.sa_flags = 0;
	sigaction(SIGINT, &int_act, NULL);
	sigaction(SIGQUIT, &quit_act, NULL);
	if (isatty(STDIN_FILENO) && tcgetattr(STDIN_FILENO, &termios_p) == 0)
	{
		termios_p.c_lflag &= ~(ECHOCTL);
		tcsetattr(STDIN_FILENO, TCSANOW, &termios_p);
	}
}

/**
 * Configure signal behavior for heredoc subprocesses.
 *
 * A heredoc child should stop on `SIGINT` in the normal way so the parent can
 * detect the interruption through `waitpid()`. `SIGQUIT` remains ignored to
 * match the shell's prompt-level policy and avoid spurious core-dumps.
 */
void	ft_sig_heredoc(void)
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
