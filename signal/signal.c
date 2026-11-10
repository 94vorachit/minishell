/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/26 19:15:41 by vorhansa          #+#    #+#             */
/*   Updated: 2026/11/10 15:57:08 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

volatile sig_atomic_t	g_signal = 0;

/**
 * Disable echoing of control characters in interactive mode.
 *
 * This keeps the prompt clean when the user presses Ctrl+C: the shell still
 * receives `SIGINT`, but the terminal does not print `^C` itself. The call is
 * guarded by `isatty(STDIN_FILENO)` because terminal control APIs are only
 * valid when stdin is attached to a real TTY.
 *
 * Implementation:
 * - `tcgetattr()` reads the current terminal attributes
 * - `ECHOCTL` is cleared from `c_lflag`
 * - `tcsetattr()` applies the new settings immediately
 */
void	sig_disabled_display(void)
{
	struct termios	termios_p;

	if (!isatty(STDIN_FILENO))
		return ;
	if (tcgetattr(STDIN_FILENO, &termios_p))
		perror("minishell: tcgetattr");
	termios_p.c_lflag &= ~(ECHOCTL);
	if (tcsetattr(STDIN_FILENO, TCSANOW, &termios_p))
		perror("minishell: tcsetattr");
}

/**
 * Prompt-level SIGINT handler.
 *
 * When the user presses Ctrl+C while the shell is waiting for input, bash-like
 * behavior is to move to a fresh line, clear the current readline buffer, and
 * redraw the prompt. The handler records the signal in `g_signal` so the main
 * loop can convert it into the shell status `130`.
 *
 * Behavior:
 * - set `g_signal = SIGINT`
 * - print a newline
 * - call `rl_on_new_line()`
 * - clear the readline buffer
 * - redraw the prompt
 */
void	sig_reprompt(int sig)
{
	(void)sig;
	sig_disabled_display();
	g_signal = SIGINT;
	write(1, "\n", 1);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
}

/**
 * Install prompt-mode signal handlers for the interactive shell.
 *
 * - `SIGINT` uses `sig_reprompt`, which keeps readline and the prompt state in
 *   sync with the shell's exit-status logic
 * - `SIGQUIT` is ignored while the shell is idle at the prompt
 *
 * This setup is only suitable for the parent shell process, not for child
 * programs. Terminal control is disabled only when stdin is a TTY.
 */
void	sig_init(void)
{
	struct sigaction	int_act;
	struct sigaction	quit_act;

	sigemptyset(&int_act.sa_mask);
	sigemptyset(&quit_act.sa_mask);
	int_act.sa_handler = sig_reprompt;
	int_act.sa_flags = 0;
	quit_act.sa_handler = SIG_IGN;
	quit_act.sa_flags = 0;
	sigaction(SIGINT, &int_act, NULL);
	sigaction(SIGQUIT, &quit_act, NULL);
	sig_disabled_display();
}

/**
 * Notes:
 * - `SIGINT` must update `g_signal` so the prompt loop can set the shell
 *   status to `128 + SIGINT`.
 * - `SIGQUIT` is intentionally ignored while idle, matching normal shell
 *   behavior at the prompt.
 * - `sig_init()` may be called repeatedly, so it must not reset `g_signal`.
 */